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
 * 模块职责:软件模拟 I2C 总线驱动(GPIO 位操作)
 */
#ifndef __DRI_SOFT_I2C_H__
#define __DRI_SOFT_I2C_H__

#include <stdio.h>
#include <rtthread.h>
#include "rthw.h"

#include "stm32f1xx_hal.h"


#define I2C_DEBUG_LEVEL 3

#if I2C_DEBUG_LEVEL == 0
#define I2C_INFO(fmt, arg...) 
#define I2C_WARNING(fmt, arg...)
#define I2C_ERROR(fmt, arg...)

#elif I2C_DEBUG_LEVEL == 1
#define I2C_INFO(fmt, arg...)   		rt_kprintf("<<-I2C-INFO->> "fmt"\r\n",##arg)
#define I2C_WARNING(fmt, arg...)
#define I2C_ERROR(fmt, arg...)

#elif I2C_DEBUG_LEVEL == 2
#define I2C_INFO(fmt, arg...)
#define I2C_WARNING(fmt, arg...)		rt_kprintf("<<-I2C-WARNING->> "fmt"\r\n",##arg)
#define I2C_ERROR(fmt, arg...)

#elif I2C_DEBUG_LEVEL == 3
#define I2C_INFO(fmt, arg...)
#define I2C_WARNING(fmt, arg...)
#define I2C_ERROR(fmt, arg...)	 		rt_kprintf("<<-I2C-ERROR->> "fmt"\r\n",##arg)
#endif


#define I2C_WR              0x00       		/* 写标志 */
#define I2C_RD              (1 << 0)      	/* 读标志 */
#define I2C_ADDR_10BIT      (1 << 1)      	/* 10 位地址模式 */
#define I2C_NO_START        (1 << 2)      	/* 无开始条件 */
#define I2C_IGNORE_NACK     (1 << 3)      	/* 忽视 NACK */
#define I2C_NO_READ_ACK     (1 << 4)      	/* 读的时候不发送 ACK */
#define I2C_NO_STOP         (1 << 5) 		/* 传输完不发送停止信号 */

// 下面两种标志位我自己添加的
#define I2C_CONTROL_BYTE    (1 << 6) 		/* 每发送一个数据之前需要发送一个控制字节用来表示之后的数据字节是命令还是数据(有的场景会用到比如：ssd1306驱动屏) */
#define I2C_SAME_BYTE       (1 << 7) 		/* 连续发送msg.tLen个msg.sByte数据字节,发送相同字节会用到避免循环调用传输函数 */




/* i2c消息定义 */
struct I2cMessage_Struct
{
	uint8_t   *p_buf;    		// 传输指定缓冲区的数据
	uint16_t  addr;    		// 从设备地址
	uint16_t  tx_len;		 	// 需要传输的数据长度(最大65535),加了I2C_CONTROL_BYTE标志不算上该长度
	uint16_t  rx_len; 	 	// 返回成功传输的数据长度,如果加了I2C_CONTROL_BYTE标志并不会算上控制字节长度
	uint8_t   flags;	 	// 标志位
	uint8_t   control_byte;	 	// I2C_CONTROL_BYTE
	uint8_t   same_byte;	 	// I2C_SAME_BYTE
};


/* i2c总线 */
struct I2cBus_Struct
{
	GPIO_TypeDef *p_gpio;
	uint32_t gpio_rcc;
	uint16_t sda_gpio_pin;
	uint16_t scl_gpio_pin;
	
	uint32_t retries; // 发地址无响应重试次数(最大不能超过255)
	
	void (*p_delay_us)(uint32_t us);
	void (*p_lock_init)(void);
	void (*p_lock)(void);
	void (*p_unlock)(void);
};

extern struct I2cBus_Struct g_st_i2c1;

int Dri_SoftI2C_Init(void);
uint32_t Dri_SoftI2C_TransferMessages(struct I2cBus_Struct *p_bus, struct I2cMessage_Struct p_msgs[], uint32_t num);

#endif
