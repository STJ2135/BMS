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
 * 模块职责:片上 CAN 驱动(滤波器配置、收发消息队列)
 */
#ifndef __DRI_CAN_H__
#define __DRI_CAN_H__

#include <stdint.h>
#include <rtthread.h>
#include "main.h"


#define CAN_DATA_LEN_MAX	8


// CAN 收发器电源控制
typedef enum
{
	CAN_POWER_ENABLE = 0u,	// 上电
	CAN_POWER_DISABLE,		// 断电
}CanPowerStateType;


// CAN 接收消息:中断里投递到接收队列,业务线程取出
typedef struct
{
	CAN_RxHeaderTypeDef rx_header;		// 接收帧头(ID/长度等,由 HAL 填充)
	uint8_t buffer[CAN_DATA_LEN_MAX];	// 数据域
}CanRxMessage_Struct;


// CAN 发送消息:业务线程投递到发送队列,发送任务取出后发出
typedef struct
{
	CAN_TxHeaderTypeDef header;			// 发送帧头(ID/长度等)
	uint8_t buffer[CAN_DATA_LEN_MAX];	// 数据域
}CanTxMessage_Struct;



void Dri_CAN_Init(void);
void Dri_CAN_SetPowerState(CanPowerStateType new_state);


void Dri_CAN_SendData(uint32_t id_type,uint32_t std_id,uint32_t ext_id,uint8_t *p_data,uint32_t length);

rt_err_t Dri_CAN_SendMessageToQueue(CanTxMessage_Struct *p_msg);
rt_err_t Dri_CAN_RecvMessageFromQueue(CanRxMessage_Struct *p_msg, rt_int32_t timeout);


void Dri_CAN_SendTest(void);


#endif


