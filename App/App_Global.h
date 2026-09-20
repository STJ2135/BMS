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
 * 模块职责:全局参量声明
 */
#ifndef __APP_GLOBAL_H__
#define __APP_GLOBAL_H__


#include "Com_Type.h"


// 全局参量:系统模式(监控线程维护) + 命令行许可(充/放/均衡) + 电芯与温度数量
extern BmsGlobalParam_Struct g_st_global_param;


#endif

