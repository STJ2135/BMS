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
 * 模块职责:通用工具函数实现(冒泡排序、二分查找、比较器)
 */
#include "Com_Utils.h"

// 查找一个数是否存在于数组中
int Com_Utils_BinarySearch(uint16_t *p_nums, uint8_t left, uint8_t right, uint16_t target)
{
	while (left <= right)
	{
		int mid = (right + left) / 2;

		if (p_nums[mid] == target)
		{
			return mid;
		}
		else if (p_nums[mid] < target)
		{
			left = mid + 1;
		}
		else if (p_nums[mid] > target)
		{
			right = mid - 1;
		}
	}

	return -1;
}

// 查找最后一个不大于 target 的元素下标(二分法)
// start_pos：起始位置
// end_pos：结束位置
// 返回-1：表示不存在这个数
int Com_Utils_FindLastNotGreater(uint16_t *p_nums, uint16_t start_pos, uint16_t end_pos, uint16_t target)
{
	uint16_t left = start_pos;
	uint16_t right = end_pos;

	while (left < right)
	{
		int mid = (left + right) / 2;

		if (p_nums[mid] == target)
		{
			left = mid + 1;
		}
		else if (p_nums[mid] < target)
		{
			left = mid + 1;
		}
		else if (p_nums[mid] > target)
		{
			right = mid;
		}
	}

	if ((left - 1) < start_pos)
	{
		return -1;
	}

	return left - 1;
}

// 查找第一个不小于 target 的元素下标(二分法)
// start_pos：起始位置
// end_pos：结束位置
// 返回-1：表示不存在这个数
int Com_Utils_FirstNotLess(uint16_t *p_nums, uint16_t start_pos, uint16_t end_pos, uint16_t target)
{
	uint16_t left = start_pos;
	uint16_t right = end_pos;

	while (left < right)
	{
		int mid = (left + right) / 2;

		if (p_nums[mid] == target)
		{
			right = mid;
		}
		else if (p_nums[mid] < target)
		{
			left = mid + 1;
		}
		else if (p_nums[mid] > target)
		{
			right = mid;
		}
	}

	if (right > end_pos)
	{
		return -1;
	}

	return right;
}

// 冒泡排序 float 数组
void Com_Utils_BubbleSortFloat(float *p_array, uint32_t count)
{
	float temp;
	uint32_t i, j;

	for (i = 1; i < count; i++)
	{
		for (j = 0; j < count - i; j++)
		{
			if (p_array[j] > p_array[j + 1])
			{
				temp = p_array[j];
				p_array[j] = p_array[j + 1];
				p_array[j + 1] = temp;
			}
		}
	}
}

int Com_Utils_CompareInt8(const void *p_e1, const void *p_e2)
{
	return *(int8_t *)p_e1 - *(int8_t *)p_e2;
}

int Com_Utils_CompareUint8(const void *p_e1, const void *p_e2)
{
	return *(uint8_t *)p_e1 - *(uint8_t *)p_e2;
}

int Com_Utils_CompareInt16(const void *p_e1, const void *p_e2)
{
	return *(int16_t *)p_e1 - *(int16_t *)p_e2;
}

int Com_Utils_CompareUint16(const void *p_e1, const void *p_e2)
{
	return *(uint16_t *)p_e1 - *(uint16_t *)p_e2;
}

int Com_Utils_CompareFloat(const void *p_e1, const void *p_e2)
{
	if (*(float *)p_e1 > *(float *)p_e2)
	{
		return 1;
	}

	return 0;
}

int Com_Utils_CompareDouble(const void *p_e1, const void *p_e2)
{
	if (*(double *)p_e1 > *(double *)p_e2)
	{
		return 1;
	}

	return 0;
}

// 交换任意类型的两个元素
void Com_Utils_Swap(uint8_t *p_buf1, uint8_t *p_buf2, uint32_t width)
{
	uint8_t temp;
	uint32_t i;

	for (i = 0; i < width; i++)
	{
		temp = *p_buf1;
		*p_buf1 = *p_buf2;
		*p_buf2 = temp;
		p_buf1++;
		p_buf2++;
	}
}

// 冒泡排序任意类型
// p_base：基地址
// size：要排序元素个数
// width：单个元素的宽度
// p_compare：不明确类型的情况下，两个数据的对比结果必须由用户完成
//            如果 p_e1 比 p_e2 大则返回大于 0 的数，反之返回小于等于 0 的数
void Com_Utils_BubbleSort(void *p_base, uint32_t size, uint32_t width, int (*p_compare)(void *p_e1, void *p_e2))
{
	uint32_t i = 0, j = 0;

	for (i = 1; i < size; i++)
	{
		for (j = 0; j < size - i; j++)
		{
			if (p_compare((uint8_t *)p_base + j * width, (uint8_t *)p_base + (j + 1) * width) > 0)
			{
				Com_Utils_Swap((uint8_t *)p_base + j * width, (uint8_t *)p_base + (j + 1) * width, width);
			}
		}
	}
}
