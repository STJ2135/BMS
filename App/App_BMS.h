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
 * 模块职责:应用入口声明
 */
#ifndef __APP_BMS_H__
#define __APP_BMS_H__


// 应用入口:组装 AFE 配置 -> 初始化硬件 -> 启动各业务线程(main 调用一次)
void App_BMS_Init(void);

#endif

