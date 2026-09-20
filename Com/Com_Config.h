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
 * 模块职责:通用/硬件相关配置宏(电芯与温度通道数量、温度测量范围)
 */
#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

#include <stdint.h>

// 最多支持多少节电芯
// BQ76920:3~5
// BQ76930:6~10
// BQ76940:9~15
#define BMS_CELL_MAX	5

// 最多支持几路温度
// BQ76920:1
// BQ76930:2
// BQ76940:3
#define BMS_TEMP_MAX	1

// 温度测量范围,具体值需要根据热敏电阻和BQ芯片测量范围决定
#define BMS_TEMP_MEASURE_MAX	125
#define BMS_TEMP_MEASURE_MIN	-55

// 温度无效值(预留):当前驱动是直接过滤掉超范围的温度,并未使用该值
#define BMS_TEMP_INVALID_VALUE	255

#endif
