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
 * 模块职责:片上 CAN 驱动实现
 */
#include <string.h>

#include "Dri_CAN.h"

#include "can.h"


#define DBG_TAG "Dri_CAN"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"



static rt_mq_t s_p_rx_mq, s_p_tx_mq;



static void Dri_CAN_SendTaskEntry(void *parameter);
static void Dri_CAN_OsResourceInit(void);
static void Dri_CAN_LlInit(void);



void Dri_CAN_Init(void)
{
	Dri_CAN_LlInit();
	Dri_CAN_OsResourceInit();
}



static void Dri_CAN_OsResourceInit(void)
{
	rt_thread_t p_thread;

	s_p_rx_mq = rt_mq_create("can_rx", sizeof(CanRxMessage_Struct), 3, RT_IPC_FLAG_PRIO);
	if (s_p_rx_mq == RT_NULL)
	{
		LOG_E("CAN mq create Fail!");
	}

	s_p_tx_mq = rt_mq_create("can_tx", sizeof(CanTxMessage_Struct), 16, RT_IPC_FLAG_PRIO);
	if (s_p_tx_mq == RT_NULL)
	{
		LOG_E("CAN mq create Fail!");
	}
	
	
	p_thread = rt_thread_create("can_send",
							   Dri_CAN_SendTaskEntry,
							   NULL,
							   512,
							   23,
							   20);

	if (p_thread == NULL)
	{
		LOG_E("Create Task Fail");
	}

	rt_thread_startup(p_thread);	
}


static void Dri_CAN_LlInit(void)
{
	CAN_FilterTypeDef st_filter = {0};

	Dri_CAN_SetPowerState(CAN_POWER_ENABLE);
	
	st_filter.FilterActivation = ENABLE;				// 是否使能筛选器
	st_filter.FilterMode = CAN_FILTERMODE_IDMASK;		// 筛选器模式
	st_filter.FilterScale = CAN_FILTERSCALE_32BIT;		// 筛选器的位数
	st_filter.FilterBank = 0;							// 筛选器编号
	st_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;	// 设置经过筛选后数据存储到哪个接收 FIFO
	st_filter.FilterIdLow = 0;			// CAN_FxR1 寄存器的低 16 位
	st_filter.FilterIdHigh = 0;		// CAN_FxR1 寄存器的高 16 位
	st_filter.FilterMaskIdLow = 0;		// CAN_FxR2 寄存器的低 16 位
	st_filter.FilterMaskIdHigh = 0;	// CAN_FxR2 寄存器的高 16 位

	// 配置滤波器
	if (HAL_CAN_ConfigFilter(&hcan, &st_filter) != HAL_OK)
	{
		LOG_E("CAN Config Filter Fail!");
	}

	// 打开CAN RX0、TX  中断
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);  

	// 开启CAN
	if (HAL_CAN_Start(&hcan) != HAL_OK)
	{
		LOG_E("CAN Start Fail!");
	}
}




void Dri_CAN_SetPowerState(CanPowerStateType new_state)
{
	HAL_GPIO_WritePin(CAN_PWREN_GPIO_Port, CAN_PWREN_Pin, (GPIO_PinState)new_state);
}




// 立即发送
void Dri_CAN_SendData(uint32_t id_type,uint32_t std_id,uint32_t ext_id,uint8_t *p_data,uint32_t length)
{
	uint32_t tx_mailbox;
	uint8_t buffer[8] = {0};
	
	CAN_TxHeaderTypeDef st_tx_header;
	
	st_tx_header.StdId = std_id;			// 标准ID
	st_tx_header.ExtId = ext_id;			// 扩展ID
	st_tx_header.IDE = id_type;			// ID类型
	st_tx_header.RTR = CAN_RTR_DATA;	// 数据帧/远程帧
	st_tx_header.DLC = length;			// 数据长度
	st_tx_header.TransmitGlobalTime = DISABLE;
	
	memcpy(buffer, p_data, length);
 
	if (HAL_CAN_AddTxMessage(&hcan, &st_tx_header, buffer, &tx_mailbox) != HAL_OK)
	{
		LOG_E("CAN Send Data Fail, can errcode:%08x", hcan.ErrorCode);
	}
}




// 将需要传输的CAN消息发送至消息队列
rt_err_t Dri_CAN_SendMessageToQueue(CanTxMessage_Struct *p_msg)
{
	rt_err_t err = RT_EOK;
	
	if ((err = rt_mq_send(s_p_tx_mq, p_msg, sizeof(CanTxMessage_Struct))) != RT_EOK)
	{
		LOG_W("can send msg to mq fail, mailbox full");
	}

	return err;
}


// 从消息队列里面读取一个CAN消息
rt_err_t Dri_CAN_RecvMessageFromQueue(CanRxMessage_Struct *p_msg, rt_int32_t timeout)
{
	rt_err_t err = RT_EOK;
	
	if ((err = rt_mq_recv(s_p_rx_mq, p_msg, sizeof(CanRxMessage_Struct), timeout)) != RT_EOK)
	{
		LOG_W("can recv msg fail");
	}

	return err;
}


// 处理发送消息队列的消息
static void Dri_CAN_SendTaskEntry(void *parameter)
{
	uint32_t tx_mailbox;
	CanTxMessage_Struct st_msg;

	while(1)
	{
		// 永远阻塞直至获取到消息
		rt_mq_recv(s_p_tx_mq, &st_msg, sizeof(CanTxMessage_Struct), RT_WAITING_FOREVER);
	 
		if (HAL_CAN_AddTxMessage(&hcan, &st_msg.header, st_msg.buffer, &tx_mailbox) != HAL_OK)
		{
			// 延时五个毫秒等待一个空的邮箱
			rt_thread_mdelay(5);
			
			// 将上一次未发送的消息重新发出
			if (HAL_CAN_AddTxMessage(&hcan, &st_msg.header, st_msg.buffer, &tx_mailbox) != HAL_OK)
			{
				//LOG_E("retry send fail,errcode = %d", hcan.ErrorCode);
			}
		}
	}
}



// 中断处理接收消息
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *p_hcan)
{
	uint8_t buffer[CAN_DATA_LEN_MAX];
	CAN_RxHeaderTypeDef st_rx_header;
	CanRxMessage_Struct st_rx_message;

	if(HAL_CAN_GetRxMessage(p_hcan, CAN_RX_FIFO0, &st_rx_header, buffer) != HAL_OK)
	{
		LOG_E("can read message error");

		return;
	}

	st_rx_message.rx_header = st_rx_header;
	memcpy(st_rx_message.buffer, buffer, sizeof(buffer));

	if (rt_mq_send(s_p_rx_mq, &st_rx_message, sizeof(CanRxMessage_Struct)) != RT_EOK)
	{
		LOG_E("can send mq error");
	}
}



















void Dri_CAN_SendTest(void)
{
	static uint8_t send_data[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	static uint8_t s_id_type_std = 0;


	send_data[0]++;
	send_data[1]++;
	send_data[2]++;
	send_data[3]++;
	send_data[4]++;
	send_data[5]++;
	send_data[6]++;
	send_data[7]++;
	
	if(s_id_type_std == 1)
	{
		Dri_CAN_SendData(CAN_ID_STD, 0x123, 0x321, send_data, 8);
		s_id_type_std = 0;
	}
	else
	{
		Dri_CAN_SendData(CAN_ID_EXT, 0x123, 0x321, send_data, 8);
		s_id_type_std = 1;
	}
}
