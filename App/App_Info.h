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
 * 模块职责:信息输出:对外接口声明
 */
#ifndef __APP_INFO_H__
#define __APP_INFO_H__

#include "Com_Type.h"

// 创建并启动信息输出线程(周期 2000ms)
void App_Info_Init(void);
// 打开/关闭周期打印(对应 shell 命令 open_info / close_info)
void App_Info_SetPrintEnable(BmsStateType new_state);
// 立即打印一次电池信息(对应 shell 命令 show_info)
void App_Info_Print(void);

#endif


