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
 * 模块职责:RS485 收发器接口(电源与收发方向控制)
 */
#ifndef __INT_RS485_H__
#define __INT_RS485_H__


// 收发器电源控制
typedef enum
{
	RS485_POWER_ENABLE = 0u,	// 上电
	RS485_POWER_DISABLE,		// 断电
}Rs485PowerStateType;



// 收发方向(EN 引脚电平:低=接收,高=发送)
typedef enum
{
	RS485_RX = 0u,			// 接收
	RS485_TX,				// 发送
}Rs485TransmitStateType;


// 初始化:收发器上电并进入接收状态
void Int_RS485_Init(void);
// 控制收发器电源
void Int_RS485_SetPowerState(Rs485PowerStateType new_state);
// 控制收发方向
void Int_RS485_SetTransmitState(Rs485TransmitStateType new_state);


#endif



