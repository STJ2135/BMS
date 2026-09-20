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
 * 模块职责:片上外设统一初始化实现
 */
#include "Dri_Board.h"

// 板级片上外设初始化,外部硬件(RS485 等)的初始化属于接口层
void Dri_Board_Init(void)
{
	Dri_CAN_Init();
}
