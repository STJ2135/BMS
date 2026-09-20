/*
 * SPDX-License-Identifier: CC BY-NC 4.0
 *
 * http://creativecommons.org/licenses/by-nc/4.0/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * 模块职责:BQ769X0 驱动实现
 */


#include "Int_BQ769X0.h"

#include <math.h>

#include "main.h"
#include "Dri_SoftI2C.h"


#define DBG_TAG "bq76920"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"

// 数据手册换算常数
#define BQ769X0_CC_ADC_LSB_UV		8.44F		// 库仑计 ADC 分辨率(uV/LSB)
#define BQ769X0_UV_TO_V			0.000001F	// uV 换算成 V
#define BQ769X0_TS_ADC_LSB_V		0.000382F	// 温度 ADC 分辨率(V/LSB)
#define BQ769X0_BAT_VOLTAGE_GAIN	4			// 电池总压换算系数

// 报警回调接口
static Bq769x0AlertOps_Struct s_alert_ops;


/* ADC增益 */
static float s_adc_gain_v_per_lsb = 0;		
static int16_t s_adc_gain_uv_per_lsb = 0;
static int8_t s_adc_offset_mv;

static bool s_is_die_temp_selected = false;  // 温度采样源:false=外部热敏电阻, true=芯片内部温度


// 寄存器组
static RegisterGroup_Struct s_registers = {0};


// 18650 1C放电倍率是指电池以1小时时间放完额定容量
// 假如电池额定容量为2200mAh ,那么1C放电电流是2.2A
// 18650短路电流阈值一般为电池的5C放电速率
// 短路电流一般设置为10A（搜索别人实际测过的经验值）


// 放电短路(SCD)保护阈值档位
// 档位由"目标电流 x 分流电阻"决定:目标 10A、分流电阻 5mΩ -> 10A x 5mΩ = 50mV,
// 在 44mV / 89mV 两档中应取低档(对应 RSNS = 0, 即不做倍压)
// 取 44mV 档后实际保护电流 = 44mV / 5mΩ = 8.8A
static const uint8_t s_scd_thresh_code = SCD_THRESH_89MV_44MV;


// 放电过流(OCD)保护阈值档位
// 目标 2.2A、分流电阻 5mΩ -> 2.2A x 5mΩ = 11mV,在 8mV / 17mV 两档中应取低档(RSNS = 0)
// 取 8mV 档后实际保护电流 = 8mV / 5mΩ = 1.6A(取 14mV 档则为 2.8A)
static const uint8_t s_ocd_thresh_code = OCD_THRESH_17MV_8MV;




// 分流电阻阻值,单位毫欧
static float s_shunt_resistor_ohm= 0.005;		


// 传感数据
Bq769x0SampleData_Struct g_st_bq769x0_sample_data = {0};


static void Int_BQ769X0_AlertHandler(void);
static void Int_BQ769X0_ControlSwitch(Bq769x0ControlType control_type, BmsStateType new_state);


/*********************************** GPIO *********************************************/
static void Int_BQ769X0_Ts1SetOutMode(void)
{
	GPIO_InitTypeDef st_gpio_init;

	st_gpio_init.Pin = BQ769X0_TS1_Pin;
	st_gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	st_gpio_init.Pull = GPIO_NOPULL;
	st_gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(BQ769X0_TS1_GPIO_Port, &st_gpio_init);
}

static void Int_BQ769X0_Ts1SetInMode(void)
{
	GPIO_InitTypeDef st_gpio_init;
	
	st_gpio_init.Pin = BQ769X0_TS1_Pin;
	st_gpio_init.Mode = GPIO_MODE_INPUT;
	st_gpio_init.Pull = GPIO_NOPULL;
	st_gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(BQ769X0_TS1_GPIO_Port, &st_gpio_init);
}

void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{
	if (gpio_pin == BQ769X0_ALERT_PIN)
	{
		Int_BQ769X0_AlertHandler();
	}
}
/******************************************************************************************/


/************************************** utils **********************************************/


// 热敏电阻阻值换算成温度
static float Int_BQ769X0_TempChange(float	rt)
{
	float temperature = 0;

	// 热敏电阻在T2常温下的标称阻值,我买的是10K
	float rp = 10000;

	// 该热敏电阻在开尔文温度下的,热敏电阻阻值为10K时对应的温度为25度
	float t2 = 273.15 + 25;

	// B值:3935、3950
	float bx = 3950;

	// 开尔文温度值
	float ka = 273.15;

	// 打印出热敏电阻的实时阻值可与购买链接的阻值与温度对应表对照查看
	//sprintf((char *)buffer, "%f", rt);

	temperature = 1 / (1 / t2 + log(rt / rp) / bx)- ka + 0.5;

	return temperature;
}


// CRC8校验
static uint8_t Int_BQ769X0_Crc8(uint8_t *p_data, uint8_t len, uint8_t key)
{
	uint8_t i, crc=0;
	
	while (len-- != 0)
	{
		for (i = 0x80; i != 0; i /= 2)
		{
			if ((crc & 0x80) != 0)
			{
				crc *= 2;
				crc ^= key;
			}
			else
			{
				crc *= 2;
			}

			if ((*p_data & i) != 0)
			{
				crc ^= key;
			}
		}
		p_data++;
	}
	return(crc);
}
/******************************************************************************************/


/********************************** write and read ****************************************/


static bool Int_BQ769X0_WriteRegisterByteWithCrc(uint8_t reg_addr, uint8_t data)
{
	uint8_t data_buffer[4];
	struct I2cMessage_Struct msg = {0};

	data_buffer[0] = BQ769X0_I2C_ADDR << 1;
	data_buffer[1] = reg_addr;
	data_buffer[2] = data;	
	data_buffer[3] = Int_BQ769X0_Crc8(data_buffer, 3, CRC_KEY);

	msg.addr = BQ769X0_I2C_ADDR;
	msg.flags = I2C_WR;	
	msg.p_buf = data_buffer + 1;
	msg.tx_len = 3;

	if (Dri_SoftI2C_TransferMessages(&g_st_i2c1, &msg, 1) != 1)
	{
		LOG_E("Write Register Byte With CRC Fail");

		return false;
	}

	return true;
}


static bool Int_BQ769X0_WriteBlockWithCrc(uint8_t start_address, uint8_t *buffer, uint8_t length)
{
	uint8_t index;
	uint8_t buffer_crc[32] = {0}, *p_pointer;
	struct I2cMessage_Struct msg = {0};

	p_pointer = buffer_crc;
	*p_pointer++ = BQ769X0_I2C_ADDR << 1;
	*p_pointer++ = start_address;
	*p_pointer++ = *buffer;
	*p_pointer = Int_BQ769X0_Crc8(buffer_crc, 3, CRC_KEY);

	for(index = 1; index < length; index++)
	{
		p_pointer++;
		buffer++;
		*p_pointer = *buffer;
		*(p_pointer + 1) = Int_BQ769X0_Crc8(p_pointer, 1, CRC_KEY);
		p_pointer++;
	}

	msg.addr = BQ769X0_I2C_ADDR;
	msg.flags = I2C_WR;	
	msg.p_buf = buffer_crc + 1;
	msg.tx_len = 2 * length + 1;

	if (Dri_SoftI2C_TransferMessages(&g_st_i2c1, &msg, 1) != 1)
	{
		LOG_E("Write Register Block With CRC Fail");
		return false;
	}

	return true;	
}


static bool Int_BQ769X0_ReadRegisterByteWithCrc(uint8_t reg_addr, uint8_t *data)
{  	
	uint8_t read_buffer[2], crc_input[2], crc_value;
	struct I2cMessage_Struct msg[2] = {0};

	msg[0].addr = BQ769X0_I2C_ADDR;
	msg[0].flags = I2C_WR;	
	msg[0].p_buf = &reg_addr;
	msg[0].tx_len = 1;

	msg[1].addr = BQ769X0_I2C_ADDR;
	msg[1].flags = I2C_RD;	
	msg[1].p_buf = read_buffer;
	msg[1].tx_len = 2;

	if (Dri_SoftI2C_TransferMessages(&g_st_i2c1, msg, 2) != 2)
	{
		LOG_E("Read Register Byte With CRC Fail");

		return false;
	}


	crc_input[0] = (BQ769X0_I2C_ADDR << 1) + 1;
	crc_input[1] = read_buffer[0];

	crc_value = Int_BQ769X0_Crc8(crc_input, 2, CRC_KEY);
	if (crc_value != read_buffer[1])
	{
		LOG_E("Read Register Byte CRC Check Fail");
		return false;
	}

	*data = read_buffer[0];

	return true;
}


static bool Int_BQ769X0_ReadRegisterWordWithCrc(uint8_t reg_addr, uint16_t *data)
{  	
	uint8_t read_buffer[4], crc_input[2], crc_value;
	struct I2cMessage_Struct msg[2] = {0};

	msg[0].addr = BQ769X0_I2C_ADDR;
	msg[0].flags = I2C_WR;	
	msg[0].p_buf = &reg_addr;
	msg[0].tx_len = 1;

	msg[1].addr = BQ769X0_I2C_ADDR;
	msg[1].flags = I2C_RD;	
	msg[1].p_buf = read_buffer;
	msg[1].tx_len = 4;

	if (Dri_SoftI2C_TransferMessages(&g_st_i2c1, msg, 2) != 2)
	{
		LOG_E("Read Register Word With CRC Fail");

		return false;
	}

	crc_input[0] = (BQ769X0_I2C_ADDR << 1) + 1;
	crc_input[1] = read_buffer[0];

	crc_value = Int_BQ769X0_Crc8(crc_input, 2, CRC_KEY);
	if (crc_value != read_buffer[1])
	{
		LOG_E("Read Register Word CRC 1 Check Fail");

		return false;
	}

	crc_value = Int_BQ769X0_Crc8(read_buffer + 2, 1, CRC_KEY);
	if (crc_value != read_buffer[3])
	{
		BQ769X0_ERROR("Read Register Word CRC 2 Check Fail");

		return false;
	}
	*data = (read_buffer[2] << 8) | read_buffer[0];

	return true;
}

static bool Int_BQ769X0_ReadBlockWithCrc(uint8_t reg_addr, uint8_t *buffer, uint8_t length)
{  	
	uint8_t index, crc_value, crc_input[2];
	uint8_t p_buf[32] = {0};
	uint8_t *read_data = p_buf;
	struct I2cMessage_Struct msg[2] = {0};


	msg[0].addr = BQ769X0_I2C_ADDR;
	msg[0].flags = I2C_WR;	
	msg[0].p_buf = &reg_addr;
	msg[0].tx_len = 1;

	msg[1].addr = BQ769X0_I2C_ADDR;
	msg[1].flags = I2C_RD;	
	msg[1].p_buf = read_data;
	msg[1].tx_len = length * 2;

	if (Dri_SoftI2C_TransferMessages(&g_st_i2c1, msg, 2) != 2)
	{
		LOG_E("Read Register Block With CRC Fail");
		
		return false;
	}

	crc_input[0] = (BQ769X0_I2C_ADDR << 1) + 1;
	crc_input[1] = read_data[0];

	crc_value = Int_BQ769X0_Crc8(crc_input, 2, CRC_KEY);
	read_data++;
	if (crc_value != *read_data)
	{
		LOG_E("Read Register Block CRC 1 Check Fail");
		
		return false;	
	}
	else
	{
		*buffer = *(read_data - 1);
	}

	for(index = 1; index < length; index++)
	{
		read_data++;
		crc_value = Int_BQ769X0_Crc8(read_data, 1, CRC_KEY);
		read_data++;
		buffer++;

		if (crc_value != *read_data)
		{
			LOG_E("Read Register Block CRC Check Fail");
			
			return false;		
		}
		else
		{
			*buffer = *(read_data - 1);
		}
	}

	return true;
}


/******************************************************************************************/


/**************************************** 传感数据采集 *****************************************/

/* 更新单节电芯电压 250ms更新一次 */
void Int_BQ769X0_UpdateCellVoltage(void)
{
	uint8_t index = 0;
	uint16_t i_temp = 0;
	uint8_t *p_raw_adc_data = NULL;	
	uint32_t l_temp = 0;

 	if (Int_BQ769X0_ReadBlockWithCrc(VC1_HI_BYTE, &(s_registers.VCell1.VCell1Byte.VC1_HI), BQ769X0_CELL_MAX << 1) != true)
 	{
		LOG_E("Update Cell Voltage Fail");
 	}
	

	p_raw_adc_data = &s_registers.VCell1.VCell1Byte.VC1_HI;
	for (index = 0; index < BQ769X0_CELL_MAX; index++)
	{
		i_temp = (unsigned int)(*p_raw_adc_data << 8) + *(p_raw_adc_data + 1);
		l_temp = ((unsigned long)i_temp * s_adc_gain_uv_per_lsb) / 1000;
		l_temp += s_adc_offset_mv;
		g_st_bq769x0_sample_data.cell_voltage[index] = l_temp / 1000.0;
		p_raw_adc_data += 2;
	}
}


/* 热敏电阻温度 2s更新一次 */
void Int_BQ769X0_UpdateTemperature(void)
{
	uint8_t index;
	uint16_t i_temp = 0;
	float tsx_voltage = 0;
	float tsx_resistance = 0;
	uint8_t *p_raw_adc_data = NULL;

	
	if (s_is_die_temp_selected == true)
	{
		s_is_die_temp_selected = false;
		if (Int_BQ769X0_WriteRegisterByteWithCrc(SYS_CTRL1, 0x18) != true)
	 	{
			LOG_E("Update Tsx Temperature Fail");
	 	}
		BQ769X0_DELAY(2000);
	}

	if (Int_BQ769X0_ReadBlockWithCrc(TS1_HI_BYTE, &(s_registers.TS1.TS1Byte.TS1_HI), BQ769X0_TEMP_MAX << 1) != true)
 	{
		LOG_E("Update Tsx Temperature Fail");
 	}

	p_raw_adc_data = &s_registers.TS1.TS1Byte.TS1_HI;
	for(index = 0; index < BQ769X0_TEMP_MAX; index++, p_raw_adc_data += 2)
	{
		// 读出ADC值
		i_temp = (uint16_t)(*p_raw_adc_data << 8) | *(p_raw_adc_data + 1);
		
		// 手册上公式是直接用Uv单位,在这我换成V单位
		tsx_voltage = i_temp * BQ769X0_TS_ADC_LSB_V;

		// tsx_resistance:热敏电阻阻值
		// 根据adc值算出热敏电阻阻值,单位:Ω
		tsx_resistance = (10000 * tsx_voltage) / (3.3 - tsx_voltage);

		// 根据电阻值算出对应的温度值
		g_st_bq769x0_sample_data.tsx_temperature[index] = Int_BQ769X0_TempChange(tsx_resistance);
	}
}


/* 获取ic内部温度,2s更新一次,未测试好 */
void Int_BQ769X0_UpdateDieTemperature(void)
{
	uint16_t adc_raw = 0;
	
	if (s_is_die_temp_selected == false)
	{
		s_is_die_temp_selected = true;
		if (Int_BQ769X0_WriteRegisterByteWithCrc(SYS_CTRL1, 0x10) != true)
	 	{
			LOG_E("Update Die Temperature Fail");
	 	}
		BQ769X0_DELAY(2000);
	}

	if (Int_BQ769X0_ReadRegisterWordWithCrc(TS1_HI_BYTE, &s_registers.TS1.TS1Word) != true)
 	{
		LOG_E("Update Die Temperature Fail");
 	}
	
	adc_raw = (s_registers.TS1.TS1Byte.TS1_HI << 8) | s_registers.TS1.TS1Byte.TS1_LO;
	// 注意:这里只是把内部温度传感器读数按 382uV/LSB 换算成电压(V)存入该字段,
	// 还没有做"电压 -> 温度"的换算,所以 die_temperature 目前实际存的是电压值(未使用)
	g_st_bq769x0_sample_data.die_temperature = adc_raw * 382.0 / 1000000.0;
	g_st_bq769x0_sample_data.die_temperature = 25 - ((g_st_bq769x0_sample_data.die_temperature - 1.2) / 0.0042);
}


/* 更新总电流 250ms更新一次 */
void Int_BQ769X0_UpdateCurrent(void)
{
	int32_t cc_raw;

	if (Int_BQ769X0_ReadRegisterWordWithCrc(CC_HI_BYTE, &s_registers.CC.CCWord) != true)
 	{
		LOG_E("Update Current Fail");
 	}
	
	cc_raw = s_registers.CC.CCByte.CC_HI << 8 | s_registers.CC.CCByte.CC_LO;

	/*CC Reading (in μV) = [16-bit 2’s Complement Value] × (8.44 μV/LSB) */
	if(cc_raw & 0x8000)
	{
		cc_raw = -((~cc_raw + 1) & 0xFFFF);
	}
	
	// unit is A
	g_st_bq769x0_sample_data.battery_current = ((cc_raw * BQ769X0_CC_ADC_LSB_UV) / s_shunt_resistor_ohm ) * BQ769X0_UV_TO_V;
}


/* 更新总电压 250ms更新一次 */
void Int_BQ769X0_UpdateBatteryVoltage(void)
{
	uint16_t adc_raw;

	if (Int_BQ769X0_ReadRegisterWordWithCrc(BAT_HI_BYTE, &s_registers.VBat.VBatWord) != true)
 	{
		LOG_E("Update Battery Voltage Fail");
 	}

	adc_raw = s_registers.VBat.VBatByte.BAT_HI << 8 | s_registers.VBat.VBatByte.BAT_LO;
	g_st_bq769x0_sample_data.battery_voltage = BQ769X0_BAT_VOLTAGE_GAIN * s_adc_gain_v_per_lsb * adc_raw;
	g_st_bq769x0_sample_data.battery_voltage += BQ769X0_CELL_MAX * s_adc_offset_mv; //unit is mV
	g_st_bq769x0_sample_data.battery_voltage /= 1000;
}
/*********************************************************************************************/


// 报警处理
static void Int_BQ769X0_AlertHandler(void)
{
	uint8_t reg_value = 0, write_value = 0;

	Int_BQ769X0_ReadRegisterByteWithCrc(SYS_STAT, &reg_value);
	if (reg_value & SYS_STAT_OCD_BIT)
	{
		write_value |= SYS_STAT_OCD_BIT;
		if (s_alert_ops.ocd != NULL) s_alert_ops.ocd();
	}

	if (reg_value & SYS_STAT_SCD_BIT)
	{
		write_value |= SYS_STAT_SCD_BIT;
		if (s_alert_ops.scd != NULL) s_alert_ops.scd();
	}

	if (reg_value & SYS_STAT_OV_BIT)
	{
		write_value |= SYS_STAT_OV_BIT;
		if (s_alert_ops.ov != NULL) s_alert_ops.ov();
	}	

	if (reg_value & SYS_STAT_UV_BIT)
	{
		write_value |= SYS_STAT_UV_BIT;
		if (s_alert_ops.uv != NULL) s_alert_ops.uv();
	}

	if (reg_value & SYS_STAT_OVRD_BIT)
	{
		write_value |= SYS_STAT_OVRD_BIT;
		if (s_alert_ops.ovrd != NULL) s_alert_ops.ovrd();
	}	

	if (reg_value & SYS_STAT_DEVICE_BIT)
	{
		write_value |= SYS_STAT_DEVICE_BIT;
		if (s_alert_ops.device != NULL) s_alert_ops.device();
	}
	
	if (reg_value & SYS_STAT_CC_BIT)
	{
		write_value |= SYS_STAT_CC_BIT;
		if (s_alert_ops.cc != NULL) s_alert_ops.cc();
	}		

	Int_BQ769X0_WriteRegisterByteWithCrc(SYS_STAT, write_value);
}


// 获取增益和偏移量
void Int_BQ769X0_GetAdcGainOffset(void)
{
	Int_BQ769X0_ReadRegisterByteWithCrc(ADCGAIN1, &(s_registers.ADCGain1.ADCGain1Byte));
	Int_BQ769X0_ReadRegisterByteWithCrc(ADCGAIN2, &(s_registers.ADCGain2.ADCGain2Byte));
	Int_BQ769X0_ReadRegisterByteWithCrc(ADCOFFSET, &(s_registers.ADCOffset));
	
	/*GAIN is uV/LSB,OFFSET is mV*/
	/* 下面的位移是因为GAIN数据是由两个寄存器拼接而成的 */
	s_adc_gain_v_per_lsb = (ADCGAIN_BASE + ((s_registers.ADCGain1.ADCGain1Byte & 0x0C) << 1) + ((s_registers.ADCGain2.ADCGain2Byte & 0xE0)>> 5)) / 1000.0;
	s_adc_gain_uv_per_lsb = ADCGAIN_BASE + ((s_registers.ADCGain1.ADCGain1Byte & 0x0C) << 1) + ((s_registers.ADCGain2.ADCGain2Byte & 0xE0)>> 5);

	if (s_registers.ADCOffset <= 0x7F) 
	{
		s_adc_offset_mv = s_registers.ADCOffset;    // 正数，直接返回
	}
	else
	{
		s_adc_offset_mv = s_registers.ADCOffset - 256;  // 负数，手动处理
	}
	
}


// 配置寄存器
static void Int_BQ769X0_Configuration(void)
{
	unsigned char read_buffer[8];

	// 开ADC,选择外部NTC
	s_registers.SysCtrl1.SysCtrl1Byte = 0x18;
	
	// 使能电流连续采样，关闭充放电MOS
	s_registers.SysCtrl2.SysCtrl2Byte = 0x40;

	// 配置CC_CFG,说明书要求在初始化时应配置为0X19以获得更好的性能
	s_registers.CCCfg = 0x19;

	// 写入配置到寄存器
	Int_BQ769X0_WriteBlockWithCrc(SYS_CTRL1, &(s_registers.SysCtrl1.SysCtrl1Byte), 8);
	Int_BQ769X0_ReadBlockWithCrc(SYS_CTRL1, read_buffer, 8);
	
	
	
	
	// 去掉BUFF[0]的最高位,防止因为接上了负载使负载检测置位而没通过校验
	if( (read_buffer[0]&0X7F) != s_registers.SysCtrl1.SysCtrl1Byte
	|| read_buffer[1] != s_registers.SysCtrl2.SysCtrl2Byte
	|| read_buffer[2] != s_registers.Protect1.Protect1Byte
	|| read_buffer[3] != s_registers.Protect2.Protect2Byte
	|| read_buffer[4] != s_registers.Protect3.Protect3Byte
	|| read_buffer[5] != s_registers.OVTrip
	|| read_buffer[6] != s_registers.UVTrip
	|| read_buffer[7] != s_registers.CCCfg)
	{
		LOG_E("BQ769X0 config register fail,Please reset BMS board");

		while(1);
	}
}


// 检测是否接了负载
// 只有在没使能充电的情况下且CHG引脚电压大于0.7V才会检测到负载
bool Int_BQ769X0_IsLoadDetected(void)
{
	Int_BQ769X0_ReadRegisterWordWithCrc(SYS_CTRL1, (uint16_t *)&s_registers.SysCtrl1.SysCtrl1Byte);
	if (s_registers.SysCtrl2.SysCtrl2Bit.CHG_ON == 0) // 不在充电状态下
	{
		if (s_registers.SysCtrl1.SysCtrl1Bit.LOAD_PRESENT)
		{
			return true;
		}
	}
	return false;
}

// 唤醒BQ芯片
void Int_BQ769X0_Wakeup(void)
{
	// 输出模式，推挽输出高电平唤醒BQ芯片
	Int_BQ769X0_Ts1SetOutMode();
	HAL_GPIO_WritePin(BQ769X0_TS1_GPIO_Port, BQ769X0_TS1_Pin, GPIO_PIN_SET);
	BQ769X0_DELAY(1000);

	// 设为输入模式，避免干扰温度采样
	HAL_GPIO_WritePin(BQ769X0_TS1_GPIO_Port, BQ769X0_TS1_Pin, GPIO_PIN_RESET);
	Int_BQ769X0_Ts1SetInMode();
	BQ769X0_DELAY(1000);
}

// 进入低功率模式
void Int_BQ769X0_EnterShipMode(void)
{
	Int_BQ769X0_WriteRegisterByteWithCrc(SYS_CTRL1, 0x00);
	Int_BQ769X0_WriteRegisterByteWithCrc(SYS_CTRL1, 0x01);
	Int_BQ769X0_WriteRegisterByteWithCrc(SYS_CTRL1, 0x02);
}

// 控制充放电开关
void Int_BQ769X0_SetCharge(BmsStateType new_state)
{
	Int_BQ769X0_ControlSwitch(CHG_CONTROL, new_state);
}

void Int_BQ769X0_SetDischarge(BmsStateType new_state)
{
	Int_BQ769X0_ControlSwitch(DSG_CONTROL, new_state);
}

static void Int_BQ769X0_ControlSwitch(Bq769x0ControlType control_type, BmsStateType new_state)
{
	if (new_state == BMS_STATE_ENABLE)
	{
		s_registers.SysCtrl2.SysCtrl2Byte |= control_type;
	}
	else
	{
		s_registers.SysCtrl2.SysCtrl2Byte &= ~control_type;
	}
	Int_BQ769X0_WriteRegisterByteWithCrc(SYS_CTRL2, s_registers.SysCtrl2.SysCtrl2Byte);
}


// 设置某个电芯均衡状态，可以位与多节，支持BQ769X0系列(相邻单元不能同时均衡)
void Int_BQ769X0_SetCellsBalance(BmsCellMaskType cell_mask, BmsStateType new_state)
{
	static uint8_t s_cell_bal_value[3] = {0};

	if (new_state == BMS_STATE_ENABLE)
	{
		s_cell_bal_value[0] |= cell_mask & 0x1F;
		s_cell_bal_value[1] |= (cell_mask >> 5) & 0x1F;
		s_cell_bal_value[2] |= (cell_mask >> 10) & 0x1F;
	}
	else if (new_state == BMS_STATE_DISABLE)
	{
		s_cell_bal_value[0] &= ~(cell_mask & 0x1F);
		s_cell_bal_value[1] &= ~((cell_mask >> 5) & 0x1F);
		s_cell_bal_value[2] &= ~((cell_mask >> 10) & 0x1F);
	}
	Int_BQ769X0_WriteBlockWithCrc(CELLBAL1, s_cell_bal_value, 3);
}


void Int_BQ769X0_SetScdDelay(BmsScdDelayType scd_delay)
{
	s_registers.Protect1.Protect1Bit.SCD_DELAY = scd_delay;
	Int_BQ769X0_WriteRegisterByteWithCrc(PROTECT1, s_registers.Protect1.Protect1Bit.SCD_DELAY);
}

void Int_BQ769X0_SetOcdDelay(BmsOcdDelayType ocd_delay)
{
	s_registers.Protect2.Protect2Bit.OCD_DELAY = ocd_delay;
	Int_BQ769X0_WriteRegisterByteWithCrc(PROTECT2, s_registers.Protect2.Protect2Bit.OCD_DELAY);
}

void Int_BQ769X0_SetUvDelay(BmsUvDelayType uv_delay)
{
	s_registers.Protect3.Protect3Bit.UV_DELAY = uv_delay;
	Int_BQ769X0_WriteRegisterByteWithCrc(PROTECT3, s_registers.Protect3.Protect3Bit.UV_DELAY);
}

void Int_BQ769X0_SetOvDelay(BmsOvDelayType ov_delay)
{
	s_registers.Protect3.Protect3Bit.OV_DELAY = ov_delay;
	Int_BQ769X0_WriteRegisterByteWithCrc(PROTECT3, s_registers.Protect3.Protect3Bit.OV_DELAY);
}

void Int_BQ769X0_SetUvpThreshold(uint16_t uvp_threshold)
{
	s_registers.UVTrip = (uint8_t)((((uint16_t)((uvp_threshold - s_adc_offset_mv)/s_adc_gain_v_per_lsb/* + 0.5*/) - UV_THRESH_BASE) >> 4) & 0xFF);
	Int_BQ769X0_WriteRegisterByteWithCrc(UV_TRIP, s_registers.UVTrip);
}

void Int_BQ769X0_SetOvpThreshold(uint16_t ovp_threshold)
{
	s_registers.OVTrip = (uint8_t)((((uint16_t)((ovp_threshold - s_adc_offset_mv)/s_adc_gain_v_per_lsb/* + 0.5*/) - OV_THRESH_BASE) >> 4) & 0xFF);
	Int_BQ769X0_WriteRegisterByteWithCrc(OV_TRIP, s_registers.OVTrip);
}


// BQ芯片初始化
void Int_BQ769X0_Init(Bq769x0InitData_Struct *p_init_data)
{
	// I2C 总线初始化(片内 GPIO 软件模拟)由接口层负责
	Dri_SoftI2C_Init();


	// 进入 SHIP(运输/断电)模式再唤醒,相当于让 AFE 走一次上电复位(POR):
	// 1) 寄存器回到出厂默认值,避免上一次运行残留的配置影响本次初始化;
	// 2) 之后读到的 ADCGAIN/ADCOFFSET 才是芯片出厂校准值(所有电压/电流换算都依赖它)
	Int_BQ769X0_EnterShipMode();
	BQ769X0_DELAY(500);
	Int_BQ769X0_Wakeup();


	// 获取增益和偏移量
	Int_BQ769X0_GetAdcGainOffset();


	s_alert_ops = p_init_data->alert_ops;


	// 配置寄存器
	s_registers.Protect1.Protect1Bit.SCD_THRESH = s_scd_thresh_code;
	s_registers.Protect2.Protect2Bit.OCD_THRESH = s_ocd_thresh_code;
	s_registers.Protect1.Protect1Bit.SCD_DELAY  = p_init_data->config_data.scd_delay;	
	s_registers.Protect2.Protect2Bit.OCD_DELAY  = p_init_data->config_data.ocd_delay;	
	s_registers.Protect3.Protect3Bit.UV_DELAY   = p_init_data->config_data.uv_delay;	
	s_registers.Protect3.Protect3Bit.OV_DELAY   = p_init_data->config_data.ov_delay;

	// BQ阈值寄存器内部比较是14位的，但我们真实写入的值是“10-XXXX-XXXX–1000”中间x的数据，所以下面计算出14位数据后需要得到中间8位再写入
	// 减去OV_THRESH_BASE就是得到中间8位，，在数据手册7.3.1.2.1章节有讲解
	s_registers.OVTrip = (uint8_t)((((uint16_t)((p_init_data->config_data.ovp_threshold - s_adc_offset_mv)/s_adc_gain_v_per_lsb/* + 0.5*/) - OV_THRESH_BASE) >> 4) & 0xFF);
	// BQ阈值寄存器内部比较是14位的，但我们真实写入的值是“01-XXXX-XXXX–0000”中间x的数据，所以下面计算出14位数据后需要得到中间8位再写入
	// 减去UV_THRESH_BASE就是得到中间8位，在数据手册7.3.1.2.1章节有讲解
	s_registers.UVTrip = (uint8_t)((((uint16_t)((p_init_data->config_data.uvp_threshold - s_adc_offset_mv)/s_adc_gain_v_per_lsb/* + 0.5*/) - UV_THRESH_BASE) >> 4) & 0xFF);


	Int_BQ769X0_Configuration();
	
	
	LOG_I("BQ769X0 Initialize successful!");
}
