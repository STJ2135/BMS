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
 * 模块职责:板载 LED 接口(4 路容量指示灯 + 系统心跳灯)
 */
#ifndef __INT_LED_H__
#define __INT_LED_H__

#include <stdbool.h>
#include <stdint.h>

// 容量指示灯编号;末尾的 MAX 表示灯的数量,不是可用编号
typedef enum
{
	INT_LED_INDEX1 = 0,	// 第 1 路(对应最低容量档)
	INT_LED_INDEX2,
	INT_LED_INDEX3,
	INT_LED_INDEX4,		// 第 4 路(对应最高容量档)
	INT_LED_INDEX_MAX,	// 灯数量
}IntLedIndexType;

// 设置某一路容量指示灯:is_on 为 true 点亮,false 熄灭
void Int_LED_SetState(IntLedIndexType led_index, bool is_on);
// 翻转系统心跳指示灯(主循环调用)
void Int_LED_ToggleSystem(void);

#endif
