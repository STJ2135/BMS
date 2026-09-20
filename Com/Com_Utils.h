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
 * 模块职责:通用工具函数声明(排序、二分查找、比较器)
 */
#ifndef __COM_UTILS_H__
#define __COM_UTILS_H__

#include <stdint.h>

// 冒泡排序(升序)
void Com_Utils_BubbleSortFloat(float *p_array, uint32_t count);

// 在 [left, right] 闭区间内查找精确匹配;返回下标,找不到返回 -1(要求数组升序)
int Com_Utils_BinarySearch(uint16_t *p_nums, uint8_t left, uint8_t right, uint16_t target);
// 返回最后一个"不大于 target"的元素下标;若第一个元素就大于 target 则返回 -1(要求数组升序)
int Com_Utils_FindLastNotGreater(uint16_t *p_nums, uint16_t start_pos, uint16_t end_pos, uint16_t target);
// 返回第一个"不小于 target"的元素下标(要求数组升序,且调用方保证 target 在区间值域内)
int Com_Utils_FirstNotLess(uint16_t *p_nums, uint16_t start_pos, uint16_t end_pos, uint16_t target);

// 各类比较器:供 Com_Utils_BubbleSort 使用,返回值 >0 表示 p_e1 应排在后面
int Com_Utils_CompareInt8(const void *p_e1, const void *p_e2);
int Com_Utils_CompareUint8(const void *p_e1, const void *p_e2);
int Com_Utils_CompareInt16(const void *p_e1, const void *p_e2);
int Com_Utils_CompareUint16(const void *p_e1, const void *p_e2);
int Com_Utils_CompareFloat(const void *p_e1, const void *p_e2);
int Com_Utils_CompareDouble(const void *p_e1, const void *p_e2);

// 按字节交换两个元素(width = 元素字节数)
void Com_Utils_Swap(uint8_t *p_buf1, uint8_t *p_buf2, uint32_t width);
// 任意类型冒泡排序(升序);p_compare 返回 >0 表示 p_e1 应排在后面
void Com_Utils_BubbleSort(void *p_base, uint32_t size, uint32_t width, int (*p_compare)(void *p_e1, void *p_e2));

#endif
