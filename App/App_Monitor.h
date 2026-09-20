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
 * 模块职责:电池数据监控:采样数据结构与对外接口
 */
#ifndef __APP_MONITOR_H__
#define __APP_MONITOR_H__


#include <stdbool.h>

#include "Com_Type.h"


typedef struct
{
	float cell_voltage; 		// 电芯电压(V)
	uint32_t cell_index;	// 电芯编号(0 起)
}BmsCellData_Struct;


typedef struct
{
	float cell_temp[BMS_TEMP_MAX];					// 采样温度,温度数据会从小到大排序(℃)
	float battery_voltage;							// 电池总电压(V)
	float battery_current;							// 电池组电流(A)
	BmsCellData_Struct cell_data_sorted[BMS_CELL_MAX];	// 按电压从小到大排序的电芯数据(含原始编号)
	float cell_voltage_by_index[BMS_CELL_MAX];			// 按物理顺序排列的电芯电压(未排序)
	uint32_t cell_temp_effective_number;				// 有效值的温度数量
}BmsMonitorData_Struct;


extern BmsMonitorData_Struct g_st_monitor_data;


// 创建并启动监控线程(周期 250ms)
void App_Monitor_Init(void);
// 使能/禁止"电芯电压"采样(默认使能;当前工程未调用,预留接口)
void App_Monitor_SetCellVoltageEnable(BmsStateType new_state);
// 使能/禁止"电池总压"采样(同上)
void App_Monitor_SetBatteryVoltageEnable(BmsStateType new_state);
// 使能/禁止"电池电流"采样(同上)
void App_Monitor_SetBatteryCurrentEnable(BmsStateType new_state);
// 使能/禁止"电芯温度"采样(同上)
void App_Monitor_SetCellTemperatureEnable(BmsStateType new_state);

// AFE 库仑计采样完成回调,在 ALERT 中断中调用(仅置位标志)
void App_Monitor_OnCcSampleReady(void);


#endif


