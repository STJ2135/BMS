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
#ifndef __APP_POWER_H__
#define __APP_POWER_H__

#include "Com_Type.h"

/*
 * 充放电输出(充电/放电 MOS 开关)的唯一所有者
 *
 * 实际输出由三层逐级"否决"得到,每一层只能收紧、不能放宽:
 *
 *   用户许可   g_st_global_param.charge_allowed    人      : open_chg / close_chg
 *        |  只能收紧
 *   策略请求   s_charge_requested                  能量管理: 按 模式/SOC/报警 细化
 *        |  只能收紧
 *   保护否决   s_charge_locked                     保护    : 故障期间一律关断
 *        v
 *   实际输出   Int_BQ769X0_SetCharge()
 *
 * 实际输出 = locked ? 关断 : requested
 *
 * 三者回答的是不同的问题,不能合并:
 *   allowed   允不允许充电      (用户)
 *   requested 现在要不要充电    (能量管理策略)
 *   locked    能不能充电        (有没有故障)
 *
 * 其它模块只提交"请求"或"保护锁定",由本模块统一决定并下发实际输出。
 */

void App_Power_Init(void);

// 常规请求:能量管理按 SOC/模式发起的期望输出状态
void App_Power_RequestCharge(BmsStateType state);
void App_Power_RequestDischarge(BmsStateType state);

// 保护锁定:故障时强制关断
void App_Power_LockCharge(void);
void App_Power_LockDischarge(void);

// 保护解除(恢复正常):解除锁定并立即恢复为命令行设置的使能状态
// 用于"由本工程主动关断"的软件保护(过流/过温/低温等)
void App_Power_ReleaseCharge(void);
void App_Power_ReleaseDischarge(void);

// 保护解除(仅解锁):解除锁定但不主动改动输出,由后续请求决定何时重新使能
// 用于 OV/UV 这类"由 AFE 自主关断、且恢复时机与 SOC/模式相关"的硬件保护
void App_Power_UnlockCharge(void);
void App_Power_UnlockDischarge(void);

// 同步命令行(open_chg / close_chg / open_dsg / close_dsg)对使能许可的修改
// 规则:关闭立即生效;开启仅在睡眠模式下立即生效,其余交给能量管理按策略判定
// 由能量管理线程周期调用
void App_Power_ApplyCommandChange(void);

#endif
