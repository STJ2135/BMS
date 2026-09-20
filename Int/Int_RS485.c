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
 * 模块职责:RS485 接口实现(方向切换 + UART 收发中断)
 */
#include "Int_RS485.h"

#include "main.h"

#include "usart.h"

#include <rtthread.h>



#define DBG_TAG "Int_RS485"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"



static uint8_t s_rc;

void Int_RS485_Init(void)
{
	Int_RS485_SetPowerState(RS485_POWER_ENABLE);



	Int_RS485_SetTransmitState(RS485_RX);
	HAL_UART_Receive_IT(&huart2, &s_rc, 1);
}

void Int_RS485_SetPowerState(Rs485PowerStateType new_state)
{
	HAL_GPIO_WritePin(RS485_PWREN_GPIO_Port, RS485_PWREN_Pin, (GPIO_PinState)new_state);
}


void Int_RS485_SetTransmitState(Rs485TransmitStateType new_state)
{
	HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, (GPIO_PinState)new_state);
}







void HAL_UART_RxCpltCallback(UART_HandleTypeDef *p_huart)
{
	if(p_huart->Instance == USART2)
	{
		LOG_D("rs485 rx done");
		Int_RS485_SetTransmitState(RS485_TX);
		HAL_UART_Transmit_IT(&huart2, &s_rc, 1);
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *p_huart)
{
	if(p_huart->Instance == USART2)
	{
		LOG_D("rs485 tx done");
		Int_RS485_SetTransmitState(RS485_RX);
		HAL_UART_Receive_IT(&huart2, &s_rc, 1);
	}
}



