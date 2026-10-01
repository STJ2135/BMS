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
 * 模块职责:通信协议层(CAN 自定义协议:状态周期上报 + 控制命令)
 */
#ifndef __MID_COMM_H__
#define __MID_COMM_H__


// CAN 协议帧 ID(标准帧,11 位)
#define MID_COMM_FRAME_CMD		0x100	// 主控 -> BMS:控制命令帧
#define MID_COMM_FRAME_ACK		0x180	// BMS -> 主控:命令应答帧
#define MID_COMM_FRAME_STATUS	0x200	// BMS -> 主控:状态周期上报帧


// 控制对象(命令帧 byte0)
#define MID_COMM_OBJ_CHARGE		0x01	// 充电
#define MID_COMM_OBJ_DISCHARGE	0x02	// 放电
#define MID_COMM_OBJ_BALANCE	0x03	// 均衡


// 控制动作(命令帧 byte1)
#define MID_COMM_ACT_DISABLE	0x00	// 关闭
#define MID_COMM_ACT_ENABLE		0x01	// 打开


void Mid_Comm_Init(void);


#endif