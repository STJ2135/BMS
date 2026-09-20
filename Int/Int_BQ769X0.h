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
 * 模块职责:BQ769X0 电池管理前端芯片驱动(寄存器访问/采样/保护配置/报警分发)
 */
#ifndef __INT_BQ769X0_H__
#define __INT_BQ769X0_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stm32f1xx_hal.h"

#include "Com_Config.h"
#include "Com_Type.h"

// BQ769X0 保护延时寄存器取值(写入 PROTECT1/2/3 对应字段)
typedef enum
{
	BMS_SCD_DELAY_50US   = 0x00,
	BMS_SCD_DELAY_100US  = 0x01,
	BMS_SCD_DELAY_200US  = 0x02,
	BMS_SCD_DELAY_400US  = 0x03,
} BmsScdDelayType;

typedef enum
{
	BMS_OCD_DELAY_10MS   = 0x00,
	BMS_OCD_DELAY_20MS   = 0x01,
	BMS_OCD_DELAY_40MS   = 0x02,
	BMS_OCD_DELAY_80MS   = 0x03,
	BMS_OCD_DELAY_160MS  = 0x04,
	BMS_OCD_DELAY_320MS  = 0x05,
	BMS_OCD_DELAY_640MS  = 0x06,
	BMS_OCD_DELAY_1280MS = 0x07,
} BmsOcdDelayType;

typedef enum
{
	BMS_OV_DELAY_1S = 0x00,
	BMS_OV_DELAY_2S = 0x01,
	BMS_OV_DELAY_4S = 0x02,
	BMS_OV_DELAY_8S = 0x03,
} BmsOvDelayType;

typedef enum
{
	BMS_UV_DELAY_1S  = 0x00,
	BMS_UV_DELAY_4S  = 0x01,
	BMS_UV_DELAY_8S  = 0x02,
	BMS_UV_DELAY_16S = 0x03,
} BmsUvDelayType;



#define BQ769X0_I2C_ADDR	0x08

#define CRC_KEY 0x07

#define LOW_BYTE(Data)			(uint8_t)(0XFF & Data)
#define HIGH_BYTE(Data)			(uint8_t)(0XFF & (Data >> 8))

#define BQ769X0_DELAY(ms)		rt_thread_mdelay(ms)


/********************************* pin config **************************/
// wake up: BQ769X0_TS1_GPIO_Port / BQ769X0_TS1_Pin 由 CubeMX 生成的 main.h 提供

// ALERT
#define BQ769X0_ALERT_GPIO_PORT	GPIOB
#define BQ769X0_ALERT_PIN		GPIO_PIN_12
#define BQ769X0_ALERT_EXIT_IRQ	EXTI15_10_IRQn
/***********************************************************************/


/********************************* cell and temp **********************/

// BQ76920	cell :3~5   温度:1
// BQ76930	cell :6~10  温度:2
// BQ76940	cell :9~15  温度:3
#define BQ769X0_CELL_MAX 	BMS_CELL_MAX	// 最大支持多少串
#define BQ769X0_TEMP_MAX	BMS_TEMP_MAX	// 最多几路温度

/****************************************************************************/




/********************************* BQ769X0 DEBUG **************************/


#define BQ769X0_DEBUG_LEVEL	1


#if (BQ769X0_DEBUG_LEVEL == 0)

#define BQ769X0_ERROR(...)		do{}while(0)
#define BQ769X0_WARNING(...)	do{}while(0)
#define BQ769X0_INFO(...)		do{}while(0)

#elif (BQ769X0_DEBUG_LEVEL == 1)

#define BQ769X0_ERROR(fmt, arg...)                              \
		do														\
		{                                                       \
			rt_kprintf("[BQ769X0 ERROR]");                  	\
			rt_kprintf(fmt"\r\n", ##arg);                     	\
		} while(0)
#define BQ769X0_WARNING(fmt, arg...)                            \
		do														\
		{                                                       \
			rt_kprintf("[BQ769X0 WARNING]");                 	\
			rt_kprintf(fmt"\r\n", ##arg);                      	\
		} while(0)
#define BQ769X0_INFO(fmt, arg...)                               \
		do														\
		{                                                       \
			rt_kprintf("[BQ769X0 INFO]");                  		\
			rt_kprintf(fmt"\r\n", ##arg);                      	\
		} while(0)


#elif (BQ769X0_DEBUG_LEVEL == 2)

#define BQ769X0_ERROR(fmt, arg...)                              \
		do														\
		{                                                       \
			rt_kprintf("[BQ769X0 ERROR][%s:%s:%d] ",            \
					__FILE__, __FUNCTION__, __LINE__);          \
			rt_kprintf(fmt"\r\n", ##arg);                      	\
		} while(0)
#define BQ769X0_WARNING(fmt, arg...)                            \
		do														\
		{                                                       \
			rt_kprintf("[BQ769X0 WARNING][%s:%s:%d] ",          \
					__FILE__, __FUNCTION__, __LINE__);          \
			rt_kprintf(fmt"\r\n", ##arg);                      	\
		} while(0)
#define BQ769X0_INFO(fmt, arg...)                               \
		do														\
		{                                                       \
			rt_kprintf("[BQ769X0 INFO][%s:%s:%d] ",             \
					__FILE__, __FUNCTION__, __LINE__);          \
			rt_kprintf(fmt"\r\n", ##arg);						\
		} while(0)
		
#endif

		

/********************************************************************************/



// 寄存器地址(BQ769X0 数据手册 8.5 寄存器映射)
#define SYS_STAT 				0x00	// 状态寄存器:各类报警标志(写 1 清除)
#define CELLBAL1 				0x01	// 电芯均衡控制 1(CELL1~5)
#define CELLBAL2 				0x02	// 电芯均衡控制 2(CELL6~10)
#define CELLBAL3 				0x03	// 电芯均衡控制 3(CELL11~15)
#define SYS_CTRL1 				0x04	// 系统控制 1:ADC/温度源/关断位
#define SYS_CTRL2 				0x05	// 系统控制 2:充放电 MOS 开关、库仑计使能
#define PROTECT1 				0x06	// 放电短路阈值/延时
#define PROTECT2 				0x07	// 放电过流阈值/延时
#define PROTECT3 				0x08	// 过压/欠压延时
#define OV_TRIP 				0x09	// 过压保护阈值(14 位档位的中间 8 位)
#define UV_TRIP 				0x0A	// 欠压保护阈值(同上)
#define CC_CFG  				0x0B	// 库仑计配置(需写 0x19)
#define VC1_HI_BYTE	 			0x0C
#define CC_HI_BYTE				0x32
#define CC_LO_BYTE				0x33
#define BAT_HI_BYTE				0x2A
#define TS1_HI_BYTE				0x2C
#define TS2_HI_BYTE				0x2E

#define ADCGAIN1 				0x50	// ADC 增益高位(出厂校准值)
#define ADCOFFSET 				0x51	// ADC 偏移(出厂校准值)
#define ADCGAIN2 				0x59	// ADC 增益低位(出厂校准值)
// SCD 阈值档位:名称中第一个数为 RSNS=1(倍压)时的阈值,第二个为 RSNS=0 时的阈值
#define SCD_THRESH_44MV_22MV	0x00
#define SCD_THRESH_67MV_33MV	0x01
#define SCD_THRESH_89MV_44MV	0x02
#define SCD_THRESH_111MV_56MV	0x03
#define SCD_THRESH_133MV_67MV	0x04
#define SCD_THRESH_155MV_68MV	0x05
#define SCD_THRESH_178MV_89MV	0x06
#define SCD_THRESH_200MV_100MV	0x07

// OCD 阈值档位:含义同上(第一个数 RSNS=1,第二个数 RSNS=0)
#define OCD_THRESH_17MV_8MV		0x00
#define OCD_THRESH_22MV_11MV	0x01
#define OCD_THRESH_28MV_14MV	0x02
#define OCD_THRESH_33MV_17MV	0x03
#define OCD_THRESH_39MV_19MV	0x04
#define OCD_THRESH_44MV_22MV	0x05
#define OCD_THRESH_50MV_25MV	0x06
#define OCD_THRESH_56MV_28MV	0x07
#define OCD_THRESH_61MV_31MV	0x08
#define OCD_THRESH_67MV_33MV	0x09
#define OCD_THRESH_72MV_36MV	0x0A
#define OCD_THRESH_78MV_39MV	0x0B
#define OCD_THRESH_83MV_42MV	0x0C
#define OCD_THRESH_89MV_44MV	0x0D
#define OCD_THRESH_94MV_47MV	0x0E
#define OCD_THRESH_100MV_50MV	0x0F




#define OV_THRESH_BASE			0x2008
#define UV_THRESH_BASE			0x1000
#define OV_STEP					0x10
#define UV_STEP					0x10

#define ADCGAIN_BASE			365



#define SYS_STAT_OCD_BIT		0X01
#define SYS_STAT_SCD_BIT		0X02
#define SYS_STAT_OV_BIT			0X04
#define SYS_STAT_UV_BIT			0X08
#define SYS_STAT_OVRD_BIT		0X10
#define SYS_STAT_DEVICE_BIT		0X20
#define SYS_STAT_CC_BIT			0X80



typedef struct _Register_Group
{
	union
	{
		struct
		{
			uint8_t OCD				:1;
			uint8_t SCD				:1;
			uint8_t OV				:1;
			uint8_t UV				:1;
			uint8_t OVRD_ALERT		:1;
			uint8_t DEVICE_XREADY	:1;
			uint8_t WAKE			:1;
			uint8_t CC_READY		:1;
		}StatusBit;
		uint8_t StatusByte;
	}SysStatus;

	union
	{
		struct
		{
			uint8_t RSVD		:3;
			uint8_t CB5			:1;
			uint8_t CB4			:1;
			uint8_t CB3			:1;
			uint8_t CB2			:1;
			uint8_t CB1			:1;
		}CellBal1Bit;
		uint8_t CellBal1Byte;
	}CellBal1;

	union
	{
		struct
		{
			uint8_t RSVD		:3;
			uint8_t CB10		:1;
			uint8_t CB9			:1;
			uint8_t CB8			:1;
			uint8_t CB7			:1;
			uint8_t CB6			:1;
		}CellBal2Bit;
		uint8_t CellBal2Byte;
	}CellBal2;

	union
	{
		struct
		{
			uint8_t RSVD			:3;
			uint8_t CB15			:1;
			uint8_t CB14			:1;
			uint8_t CB13			:1;
			uint8_t CB12			:1;
			uint8_t CB11			:1;
		}CellBal3Bit;
		uint8_t CellBal3Byte;
	}CellBal3;

	union
	{
		struct
		{
			uint8_t SHUT_B			:1;
			uint8_t SHUT_A			:1;
			uint8_t RSVD1			:1;
			uint8_t TEMP_SEL		:1;
			uint8_t ADC_EN			:1;
			uint8_t RSVD2			:2;
			uint8_t LOAD_PRESENT	:1;
		}SysCtrl1Bit;
		uint8_t SysCtrl1Byte;
	}SysCtrl1;

	union
	{
		struct
		{
			uint8_t CHG_ON			:1;
			uint8_t DSG_ON			:1;
			uint8_t WAKE_T			:2;
			uint8_t WAKE_EN			:1;
			uint8_t CC_ONESHOT		:1;
			uint8_t CC_EN			:1;
			uint8_t DELAY_DIS		:1;
		}SysCtrl2Bit;
		uint8_t SysCtrl2Byte;
	}SysCtrl2;

	union
	{
		struct
		{
			uint8_t SCD_THRESH		:3;
			uint8_t SCD_DELAY		:2;
			uint8_t RSVD			:2;
			uint8_t RSNS			:1;
		}Protect1Bit;
		uint8_t Protect1Byte;
	}Protect1;

	union
	{
		struct
		{
			uint8_t OCD_THRESH		:4;
			uint8_t OCD_DELAY		:3;
			uint8_t RSVD			:1;
		}Protect2Bit;
		uint8_t Protect2Byte;
	}Protect2;

	union
	{
		struct
		{
			uint8_t RSVD			:4;
			uint8_t OV_DELAY		:2;
			uint8_t UV_DELAY		:2;
		}Protect3Bit;
		uint8_t Protect3Byte;
	}Protect3;

	uint8_t OVTrip;
	uint8_t UVTrip;
	uint8_t CCCfg;			//must be 0x19

	union
	{
		struct
		{
			uint8_t VC1_HI;
			uint8_t VC1_LO;
		}VCell1Byte;
		uint16_t VCell1Word;
	}VCell1;

	union
	{
		struct
		{
			uint8_t VC2_HI;
			uint8_t VC2_LO;
		}VCell2Byte;
		uint16_t VCell2Word;
	}VCell2;

	union
	{
		struct
		{
			uint8_t VC3_HI;
			uint8_t VC3_LO;
		}VCell3Byte;
		uint16_t VCell3Word;
	}VCell3;

	union
	{
		struct
		{
			uint8_t VC4_HI;
			uint8_t VC4_LO;
		}VCell4Byte;
		uint16_t VCell4Word;
	}VCell4;

	union
	{
		struct
		{
			uint8_t VC5_HI;
			uint8_t VC5_LO;
		}VCell5Byte;
		uint16_t VCell5Word;
	}VCell5;

	union
	{
		struct
		{
			uint8_t VC6_HI;
			uint8_t VC6_LO;
		}VCell6Byte;
		uint16_t VCell6Word;
	}VCell6;

	union
	{
		struct
		{
			uint8_t VC7_HI;
			uint8_t VC7_LO;
		}VCell7Byte;
		uint16_t VCell7Word;
	}VCell7;

	union
	{
		struct
		{
			uint8_t VC8_HI;
			uint8_t VC8_LO;
		}VCell8Byte;
		uint16_t VCell8Word;
	}VCell8;

	union
	{
		struct
		{
			uint8_t VC9_HI;
			uint8_t VC9_LO;
		}VCell9Byte;
		uint16_t VCell9Word;
	}VCell9;

	union
	{
		struct
		{
			uint8_t VC10_HI;
			uint8_t VC10_LO;
		}VCell10Byte;
		uint16_t VCell10Word;
	}VCell10;

	union
	{
		struct
		{
			uint8_t VC11_HI;
			uint8_t VC11_LO;
		}VCell11Byte;
		uint16_t VCell11Word;
	}VCell11;

	union
	{
		struct
		{
			uint8_t VC12_HI;
			uint8_t VC12_LO;
		}VCell12Byte;
		uint16_t VCell12Word;
	}VCell12;

	union
	{
		struct
		{
			uint8_t VC13_HI;
			uint8_t VC13_LO;
		}VCell13Byte;
		uint16_t VCell13Word;
	}VCell13;

	union
	{
		struct
		{
			uint8_t VC14_HI;
			uint8_t VC14_LO;
		}VCell14Byte;
		uint16_t VCell14Word;
	}VCell14;

	union
	{
		struct
		{
			uint8_t VC15_HI;
			uint8_t VC15_LO;
		}VCell15Byte;
		uint16_t VCell15Word;
	}VCell15;

	union
	{
		struct
		{
			uint8_t BAT_HI;
			uint8_t BAT_LO;
		}VBatByte;
		uint16_t VBatWord;
	}VBat;

	union
	{
		struct
		{
			uint8_t TS1_HI;
			uint8_t TS1_LO;
		}TS1Byte;
		uint16_t TS1Word;
	}TS1;

	union
	{
		struct
		{
			uint8_t TS2_HI;
			uint8_t TS2_LO;
		}TS2Byte;
		uint16_t TS2Word;
	}TS2;

	union
	{
		struct
		{
			uint8_t TS3_HI;
			uint8_t TS3_LO;
		}TS3Byte;
		uint16_t TS3Word;
	}TS3;

	union
	{
		struct
		{
			uint8_t CC_HI;
			uint8_t CC_LO;
		}CCByte;
		uint16_t CCWord;
	}CC;

	union
	{
		struct
		{
			uint8_t RSVD1			:2;
			uint8_t ADCGAIN_4_3		:2;
			uint8_t RSVD2			:4;
		}ADCGain1Bit;
		uint8_t ADCGain1Byte;
	}ADCGain1;

	uint8_t ADCOffset;

	union
	{
		struct
		{
			uint8_t RSVD			:5;
			uint8_t ADCGAIN_2_0		:3;
		}ADCGain2Bit;
		uint8_t ADCGain2Byte;
	}ADCGain2;

}RegisterGroup_Struct;





// AFE 初始化参数:保护延时档位 + 过压/欠压阈值
typedef struct
{
	BmsScdDelayType scd_delay;		// 放电短路(SCD)保护延时档位
	BmsOcdDelayType ocd_delay;		// 放电过流(OCD)保护延时档位
	BmsUvDelayType uv_delay;			// 欠压(UV)保护延时档位
	BmsOvDelayType ov_delay;			// 过压(OV)保护延时档位
	uint16_t uvp_threshold;			// 欠压保护阈值(mV),驱动按 ADC 增益换算成寄存器值
	uint16_t ovp_threshold;			// 过压保护阈值(mV),同上
}Bq769x0ConfigData_Struct;


// BQ硬件报警回调接口
typedef struct
{
	void (*ocd)(void);		// BQ769X0 放电过流硬件报警
	void (*scd)(void);		// BQ769X0 放电短路硬件报警
	void (*ov)(void);		// BQ769X0 充电过压硬件报警
	void (*uv)(void);		// BQ769X0 放电欠压硬件报警
	void (*ovrd)(void);		// BQ769X0 报警引脚由用户外围电路强行触发
	void (*device)(void);	// BQ769X0 设备故障报警
	void (*cc)(void);		// BQ769X0 库仑计采样完成
}Bq769x0AlertOps_Struct;

// 初始化数据结构体
typedef struct
{
	Bq769x0AlertOps_Struct alert_ops;
	Bq769x0ConfigData_Struct config_data;
}Bq769x0InitData_Struct;


typedef enum
{
	CHG_CONTROL = 0x01,
	DSG_CONTROL = 0x02
}Bq769x0ControlType;


typedef struct
{
	float cell_voltage[BQ769X0_CELL_MAX];	// 单节电芯电压
	float tsx_temperature[BQ769X0_TEMP_MAX];	// 热敏电阻温度
	float battery_current;	// 电池包总电流
	float battery_voltage;	// 电池包总电压
	float die_temperature;	// ic温度,目前还未测试成功
}Bq769x0SampleData_Struct;

extern Bq769x0SampleData_Struct g_st_bq769x0_sample_data;


// 初始化:进 SHIP 复位 AFE -> 读取出厂 ADC 校准值 -> 下发保护参数
void Int_BQ769X0_Init(Bq769x0InitData_Struct *p_init_data);
// 从 SHIP 模式唤醒(拉高 TS1 引脚)
void Int_BQ769X0_Wakeup(void);
// 进入 SHIP(运输/断电)模式:寄存器复位为出厂值
void Int_BQ769X0_EnterShipMode(void);
// 负载检测:未开启充电且 CHG 引脚电压超阈值时返回 true
bool Int_BQ769X0_IsLoadDetected(void);
// 控制充电 MOS(CHG)开关
void Int_BQ769X0_SetCharge(BmsStateType new_state);
// 控制放电 MOS(DSG)开关
void Int_BQ769X0_SetDischarge(BmsStateType new_state);
// 控制电芯均衡:cell_mask 每 bit 对应一节电芯
void Int_BQ769X0_SetCellsBalance(BmsCellMaskType cell_mask, BmsStateType new_state);

// 以下采样接口:读取 AFE 并刷新 g_st_bq769x0_sample_data
void Int_BQ769X0_UpdateCellVoltage(void);
void Int_BQ769X0_UpdateTemperature(void);
void Int_BQ769X0_UpdateDieTemperature(void);
void Int_BQ769X0_UpdateCurrent(void);
void Int_BQ769X0_UpdateBatteryVoltage(void);

// 以下为保护参数在线修改接口(初始化时已下发一次)
void Int_BQ769X0_SetScdDelay(BmsScdDelayType scd_delay);
void Int_BQ769X0_SetOcdDelay(BmsOcdDelayType ocd_delay);
void Int_BQ769X0_SetUvDelay(BmsUvDelayType uv_delay);
void Int_BQ769X0_SetOvDelay(BmsOvDelayType ov_delay);
void Int_BQ769X0_SetUvpThreshold(uint16_t uvp_threshold);
void Int_BQ769X0_SetOvpThreshold(uint16_t ovp_threshold);

#endif
