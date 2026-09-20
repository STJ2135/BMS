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
 * 模块职责:板载 LED 接口实现
 */
#include "Int_LED.h"

#include "main.h"

// 4 路电池容量指示灯
static GPIO_TypeDef * const s_p_led_port[INT_LED_INDEX_MAX] =
{
	LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port, LED4_GPIO_Port,
};

static const uint16_t s_led_pin[INT_LED_INDEX_MAX] =
{
	LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin,
};

// 设置指定容量指示灯的状态,is_on 为 true 表示点亮
void Int_LED_SetState(IntLedIndexType led_index, bool is_on)
{
	if (led_index >= INT_LED_INDEX_MAX)
	{
		return;
	}

	HAL_GPIO_WritePin(s_p_led_port[led_index], s_led_pin[led_index],
					  is_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// 翻转系统心跳指示灯
void Int_LED_ToggleSystem(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}
