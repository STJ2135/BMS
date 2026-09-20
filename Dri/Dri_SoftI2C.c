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
 * 模块职责:软件模拟 I2C 总线驱动实现
 */

/**************************************************************************/
/*********** 该i2c驱动从rtt的i2c驱动框架移植而来,剔除了device层 ***********/
/**** 考虑了并发情况,考虑到片内资源单独跑os内核的情况可以用,并不占资源 ****/
/**************************************************************************/

#include "Dri_SoftI2C.h"

#include "main.h"


// 使用互斥锁会被高优先级任务抢占
// 偶发性的导致i2c信号传输一半的时候就跑去做其他的
// 最后导致读写i2c数据不对进而BQ芯片驱动的CRC通不过

static rt_uint32_t s_level;


static void Dri_SoftI2C_LockInit(void)
{
}

static void Dri_SoftI2C_Lock(void)
{

	s_level = rt_hw_interrupt_disable();
}

static void Dri_SoftI2C_Unlock(void)
{

	rt_hw_interrupt_enable(s_level);
}

// 适用于72MHZ
static void Dri_SoftI2C_DelayUs(uint32_t us)
{
	uint16_t i = 0;
	
	while(us--)
	{
		i = 10; //自己定义
		while(i--);
	}
}

// i2c总线句柄
struct I2cBus_Struct g_st_i2c1 = 
{
	.p_gpio = I2C1_SCL_GPIO_Port,
	.sda_gpio_pin = I2C1_SDA_Pin,
	.scl_gpio_pin = I2C1_SCL_Pin,
	.retries = 3,
	.p_delay_us = (void (*)(uint32_t))Dri_SoftI2C_DelayUs,
	.p_lock_init = Dri_SoftI2C_LockInit,
	.p_lock = Dri_SoftI2C_Lock,
	.p_unlock = Dri_SoftI2C_Unlock,
};


static void Dri_SoftI2C_HardwareInit(struct I2cBus_Struct *p_bus)
{
	GPIO_InitTypeDef st_gpio_init;

	st_gpio_init.Pin = p_bus->scl_gpio_pin | p_bus->sda_gpio_pin;
	st_gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
	st_gpio_init.Pull = GPIO_NOPULL;
	st_gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(p_bus->p_gpio, &st_gpio_init);

	HAL_GPIO_WritePin(p_bus->p_gpio, p_bus->scl_gpio_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(p_bus->p_gpio, p_bus->sda_gpio_pin, GPIO_PIN_SET);
	
	if (p_bus->p_lock_init)p_bus->p_lock_init();
}


static inline void Dri_SoftI2C_SdaSetOutMode(struct I2cBus_Struct *p_bus)
{
	GPIO_InitTypeDef st_gpio_init;
	
	st_gpio_init.Pin = p_bus->sda_gpio_pin;
	st_gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
	st_gpio_init.Pull = GPIO_NOPULL;
	st_gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(p_bus->p_gpio, &st_gpio_init);
}

// stm32的IO口结构输出模式下是没有关断输入部分的肖特基触发器,数据依然会读入输入寄存器,故不用设置SDA输入模式
// 但为了保险起见最好还是写上,也为了方便将该驱动移植到其他平台或者HAL库上
static inline void Dri_SoftI2C_SdaSetInMode(struct I2cBus_Struct *p_bus)
{
	GPIO_InitTypeDef st_gpio_init;
	
	st_gpio_init.Pin = p_bus->sda_gpio_pin;
	st_gpio_init.Mode = GPIO_MODE_INPUT;
	st_gpio_init.Pull = GPIO_NOPULL;
	st_gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(p_bus->p_gpio, &st_gpio_init);
}

static inline uint8_t Dri_SoftI2C_GetSda(struct I2cBus_Struct *p_bus)
{
	return HAL_GPIO_ReadPin(p_bus->p_gpio, p_bus->sda_gpio_pin);
}

static inline void Dri_SoftI2C_SdaLow(struct I2cBus_Struct *p_bus)
{
	HAL_GPIO_WritePin(p_bus->p_gpio, p_bus->sda_gpio_pin, GPIO_PIN_RESET);
}

static inline void Dri_SoftI2C_SdaHigh(struct I2cBus_Struct *p_bus)
{
	HAL_GPIO_WritePin(p_bus->p_gpio, p_bus->sda_gpio_pin, GPIO_PIN_SET);
}

static inline void Dri_SoftI2C_SclLow(struct I2cBus_Struct *p_bus)
{
	HAL_GPIO_WritePin(p_bus->p_gpio, p_bus->scl_gpio_pin, GPIO_PIN_RESET);
}

static inline void Dri_SoftI2C_SclHigh(struct I2cBus_Struct *p_bus)
{
	HAL_GPIO_WritePin(p_bus->p_gpio, p_bus->scl_gpio_pin, GPIO_PIN_SET);
}

static inline void Dri_SoftI2C_Start(struct I2cBus_Struct *p_bus)
{
	Dri_SoftI2C_SdaLow(p_bus);
	p_bus->p_delay_us(1);
	Dri_SoftI2C_SclLow(p_bus);
}

static inline void Dri_SoftI2C_Restart(struct I2cBus_Struct *p_bus)
{
	Dri_SoftI2C_SdaHigh(p_bus);
	Dri_SoftI2C_SclHigh(p_bus);
	p_bus->p_delay_us(1);
	Dri_SoftI2C_SdaLow(p_bus);
	p_bus->p_delay_us(1);
	Dri_SoftI2C_SclLow(p_bus);
}

static inline void Dri_SoftI2C_Stop(struct I2cBus_Struct *p_bus)
{
	Dri_SoftI2C_SdaLow(p_bus);
	p_bus->p_delay_us(1);
	Dri_SoftI2C_SclHigh(p_bus);
	p_bus->p_delay_us(1);
	Dri_SoftI2C_SdaHigh(p_bus);
	p_bus->p_delay_us(1);
}

static inline uint8_t Dri_SoftI2C_WaitAck(struct I2cBus_Struct *p_bus)
{
	uint8_t ack;

	Dri_SoftI2C_SdaHigh(p_bus);
	p_bus->p_delay_us(1);
	Dri_SoftI2C_SclHigh(p_bus);
	Dri_SoftI2C_SdaSetInMode(p_bus);
	ack = !Dri_SoftI2C_GetSda(p_bus);    /* ACK : SDA pin is pulled low */
	Dri_SoftI2C_SdaSetOutMode(p_bus);
	I2C_INFO("%s", ack ? "ACK" : "NACK");
	Dri_SoftI2C_SclLow(p_bus);

	return ack;
}

static inline void Dri_SoftI2C_SendAckOrNack(struct I2cBus_Struct *p_bus, int ack)
{
	if (ack)
		Dri_SoftI2C_SdaLow(p_bus);
	p_bus->p_delay_us(1);
	Dri_SoftI2C_SclHigh(p_bus);
	Dri_SoftI2C_SclLow(p_bus);
}

static uint8_t Dri_SoftI2C_WriteByte(struct I2cBus_Struct *p_bus, uint8_t data)
{
	uint8_t mask;
	
	for (mask = 0x80; mask != 0; mask >>= 1)
	{
		Dri_SoftI2C_SclLow(p_bus);
		data & mask ? Dri_SoftI2C_SdaHigh(p_bus) : Dri_SoftI2C_SdaLow(p_bus);
		p_bus->p_delay_us(1);
		Dri_SoftI2C_SclHigh(p_bus);
	}
	Dri_SoftI2C_SclLow(p_bus);
	p_bus->p_delay_us(1);

	return Dri_SoftI2C_WaitAck(p_bus);
}


static uint8_t Dri_SoftI2C_ReadByte(struct I2cBus_Struct *p_bus)
{
	uint8_t mask;
	uint8_t data = 0;

	Dri_SoftI2C_SdaHigh(p_bus);
	p_bus->p_delay_us(1);
	Dri_SoftI2C_SdaSetInMode(p_bus);
	for (mask = 0x80; mask != 0; mask >>= 1) 
	{
		Dri_SoftI2C_SclHigh(p_bus);
		if (Dri_SoftI2C_GetSda(p_bus)) data |= mask;
		Dri_SoftI2C_SclLow(p_bus);
		p_bus->p_delay_us(1);
	}
	Dri_SoftI2C_SdaSetOutMode(p_bus);

	return data;
}

static uint16_t Dri_SoftI2C_SendBytes(struct I2cBus_Struct *p_bus, struct I2cMessage_Struct *p_msg)
{
	uint8_t ret;
	const uint8_t *p_ptr = p_msg->p_buf;
	uint16_t bytes = 0, count = p_msg->tx_len;
	
	while (count > 0)
	{
		if (p_msg->flags & I2C_CONTROL_BYTE && Dri_SoftI2C_WriteByte(p_bus, p_msg->control_byte) == 0) // 发送控制字节
		{
			I2C_WARNING("send bytes: NACK.");
			break;
		}

		ret = p_msg->flags & I2C_SAME_BYTE ? Dri_SoftI2C_WriteByte(p_bus, p_msg->same_byte) : Dri_SoftI2C_WriteByte(p_bus, *p_ptr) , p_ptr++;
		
		if ((ret > 0) || (p_msg->flags & I2C_IGNORE_NACK && (ret == 0)))
		{
			count --;			
			bytes ++;
		}
		else if (ret == 0)
		{
			I2C_WARNING("send bytes: NACK.");
			break;
		}
	}

	return bytes;
}

static uint16_t Dri_SoftI2C_RecvBytes(struct I2cBus_Struct *p_bus, struct I2cMessage_Struct *p_msg)
{
	uint8_t val;
	uint8_t *p_ptr = p_msg->p_buf;
	uint16_t bytes = 0, count = p_msg->tx_len;

	while (count > 0)
	{
		val = Dri_SoftI2C_ReadByte(p_bus);
		*p_ptr = val;
		bytes ++;
		
		p_ptr++;
		count--;

		I2C_INFO("recieve bytes: 0x%02x, %s",
							val, (p_msg->flags & I2C_NO_READ_ACK) ?
							"(No ACK/NACK)" : (count ? "ACK" : "NACK"));

		if (!(p_msg->flags & I2C_NO_READ_ACK))
		{
			Dri_SoftI2C_SendAckOrNack(p_bus, count);
		}
	}

	return bytes;
}

static uint8_t Dri_SoftI2C_SendAddress(struct I2cBus_Struct *p_bus, uint8_t addr, uint32_t retries)
{
	uint8_t i, ret = 0;

	for (i = 0; i <= retries; i++)
	{
		ret = Dri_SoftI2C_WriteByte(p_bus, addr);
		if (ret == 1)
		{
			I2C_INFO("response ok.");
			break;
		}
		else if (i == retries)
		{
			I2C_WARNING("no response, please check slave device.");
			break;
		}
		I2C_WARNING("no response, attempt to resend the address. number:%d.", i);
		Dri_SoftI2C_Stop(p_bus);
		p_bus->p_delay_us(1);
		Dri_SoftI2C_Start(p_bus);
	}

	return ret;
}

static uint8_t Dri_SoftI2C_BitSendAddress(struct I2cBus_Struct *p_bus, struct I2cMessage_Struct *p_msg)
{
	uint8_t ret, retries, addr1, addr2;
	uint8_t flags = p_msg->flags;
	uint8_t ignore_nack = p_msg->flags & I2C_IGNORE_NACK;


	retries = ignore_nack ? 0 : p_bus->retries;

	if (flags & I2C_ADDR_10BIT)
	{
		addr1 = 0xf0 | ((p_msg->addr >> 7) & 0x06);
		addr2 = p_msg->addr & 0xff;

		I2C_INFO("addr1: %d, addr2: %d", addr1, addr2);

		ret = Dri_SoftI2C_SendAddress(p_bus, addr1, retries);
		if ((ret != 1) && !ignore_nack)
		{
			I2C_WARNING("NACK: sending first addr");
			return 0;
		}

		ret = Dri_SoftI2C_WriteByte(p_bus, addr2);
		if ((ret != 1) && !ignore_nack)
		{
			I2C_WARNING("NACK: sending second addr");
			return 0;
		}
		if (flags & I2C_RD)
		{
			I2C_INFO("send repeated start condition");
			Dri_SoftI2C_Restart(p_bus);
			addr1 |= 0x01;
			ret = Dri_SoftI2C_SendAddress(p_bus, addr1, retries);
			if ((ret != 1) && !ignore_nack)
			{
				I2C_ERROR("NACK: sending repeated addr");
				return 0;
			}
		}
	}
	else
	{
		/* 7-bit addr */
		addr1 = p_msg->addr << 1;
		if (flags & I2C_RD)
				addr1 |= 1;
		ret = Dri_SoftI2C_SendAddress(p_bus, addr1, retries);
		if ((ret != 1) && !ignore_nack)
				return 0;
	}

	return 1;
}

/* 传输消息,如果传输成功返回总的消息数量,失败返回0 */
uint32_t Dri_SoftI2C_TransferMessages(struct I2cBus_Struct *p_bus, struct I2cMessage_Struct p_msgs[], uint32_t num)
{
	struct I2cMessage_Struct *p_msg;
	uint32_t i, ret = 0;
	uint8_t ignore_nack;

	if (NULL == p_bus || NULL == p_msgs || num == 0)return ret;

	if (p_bus->p_lock) p_bus->p_lock();

	for (i = 0; i < num; i++)
	{
		p_msg = &p_msgs[i];
		ignore_nack = p_msg->flags & I2C_IGNORE_NACK;
		if (!(p_msg->flags & I2C_NO_START))
		{
			if (i)
			{
				Dri_SoftI2C_Restart(p_bus);
			}
			else
			{
				I2C_INFO("send start condition");
				Dri_SoftI2C_Start(p_bus);
			}
			ret = Dri_SoftI2C_BitSendAddress(p_bus, p_msg);
			if ((ret != 1) && !ignore_nack)
			{
				I2C_WARNING("receive NACK from device addr 0x%02x msg %d", p_msgs[i].addr, i);
				goto out;
			}
		}
		if (p_msg->flags & I2C_RD)
		{
			ret = Dri_SoftI2C_RecvBytes(p_bus, p_msg);
			p_msg->rx_len = ret;
			I2C_INFO("read %d byte%s", ret, ret == 1 ? "" : "s");
		}
		else
		{
			ret = Dri_SoftI2C_SendBytes(p_bus, p_msg);	
			p_msg->rx_len = ret;
			I2C_INFO("write %d byte%s", ret, ret == 1 ? "" : "s");				
			if (p_msg->rx_len != p_msg->tx_len)
			{
				ret = 0;
				goto out;
			}
			
		}
	}
	ret = i;

out:
	if (!(p_msg->flags & I2C_NO_STOP))
	{
		I2C_INFO("send stop condition");
		Dri_SoftI2C_Stop(p_bus);
	}

	if(p_bus->p_unlock) p_bus->p_unlock();
	
	return ret;
}


int Dri_SoftI2C_Init(void)
{
	Dri_SoftI2C_HardwareInit(&g_st_i2c1);
	
	return 0;
}

