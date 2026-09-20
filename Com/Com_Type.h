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
 * 模块职责:基础类型:电芯位掩码、充放电与系统模式枚举、全局参量结构
 */
#ifndef __COM_TYPE_H__
#define __COM_TYPE_H__


#include <stdbool.h>
#include <stdint.h>

#include "Com_Config.h"





// 使能/禁止状态(也用作"许可")
typedef enum
{
	BMS_STATE_ENABLE,	// 使能/允许
	BMS_STATE_DISABLE	// 禁止/不允许
}BmsStateType;


// 电芯位掩码:每一位对应一节电芯,用于表示"哪些节电芯"
// 例:BMS_CELL_MASK1 | BMS_CELL_MASK3 表示第 1、3 节
typedef enum
{
	BMS_CELL_NONE		= 0x0000,
	BMS_CELL_MASK1 	= 0x0001,
	BMS_CELL_MASK2 	= 0x0002,
	BMS_CELL_MASK3 	= 0x0004,
	BMS_CELL_MASK4 	= 0x0008,
	BMS_CELL_MASK5 	= 0x0010,
	BMS_CELL_MASK6 	= 0x0020,
	BMS_CELL_MASK7 	= 0x0040,
	BMS_CELL_MASK8 	= 0x0080,
	BMS_CELL_MASK9 	= 0x0100,
	BMS_CELL_MASK10	= 0x0200,	
	BMS_CELL_MASK11 	= 0x0400,
	BMS_CELL_MASK12 	= 0x0800,
	BMS_CELL_MASK13 	= 0x1000,
	BMS_CELL_MASK14 	= 0x2000,
	BMS_CELL_MASK15	= 0x4000,
	BMS_CELL_ALL		= 0x7FFF,
}BmsCellMaskType;








typedef enum
{
	BMS_MODE_NULL,
	BMS_MODE_CHARGE,	// 充电模式
	BMS_MODE_DISCHARGE,	// 放电模式
	BMS_MODE_STANDBY,	// 待机模式
	BMS_MODE_SLEEP,		// 睡眠模式
}BmsSysModeType;


typedef struct
{
	BmsSysModeType sys_mode;	// 当前系统处于什么模式
	BmsStateType charge_allowed;		// 命令行授予的充电许可,不代表充电已开启
	BmsStateType discharge_allowed;	// 命令行授予的放电许可,不代表放电已开启
	BmsStateType balance_allowed;	// 命令行授予的均衡许可,不代表均衡已开启
	
	uint8_t cell_real_number;	// 电芯实时数量
	uint8_t temp_real_number;	// 温度实时数量
}BmsGlobalParam_Struct;



#endif


