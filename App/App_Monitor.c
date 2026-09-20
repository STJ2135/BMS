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
 * 模块职责:电池数据监控实现(采样调度 + 系统模式状态机)
 */
#include <stdio.h>
#include <rtthread.h>

#include "App_Monitor.h"
#include "App_Config.h"

#include "Int_BQ769X0.h"

#include "App_Energy.h"
#include "App_Global.h"

#define DBG_TAG "monitor"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"
#include "Com_Utils.h"

// p_thread config
#define MONITOR_TASK_STACK_SIZE	512
#define MONITOR_TASK_PRIORITY	19
#define MONITOR_TASK_TIMESLICE	25

#define MONITOR_TASK_PERIOD		250

// 分钟换算成毫秒
#define MONITOR_MIN_TO_MS(min)	((min) * 60 * 1000)

// 待机判定电流阈值(A)
#define MONITOR_STANDBY_CURRENT_POS_A	0.02F
#define MONITOR_STANDBY_CURRENT_NEG_A	-0.02F

// sample time config  MS
#define UPDATE_CELL_VOLTAGE_PERIOD_MS	250
#define UPDATE_BATTERY_VOLTAGE_PERIOD_MS	250
#define UPDATE_CELL_TEMP_PERIOD_MS		2000

BmsMonitorData_Struct g_st_monitor_data;

static volatile bool s_is_cc_sample_ready = false;

static bool s_is_cell_voltage_enabled = true;
static bool s_is_battery_voltage_enabled = true;
static bool s_is_cell_temp_enabled = true;
static bool s_is_battery_current_enabled = true;

static uint16_t s_cell_voltage_elapsed_ms = 0;
static uint16_t s_battery_voltage_elapsed_ms = 0;
static uint16_t s_cell_temp_elapsed_ms = 0;

static BmsSysModeType s_prev_sys_mode = BMS_MODE_NULL;
static uint32_t s_standby_elapsed_ms = 0;

static void App_Monitor_TaskEntry(void *parameter);
static void App_Monitor_SampleBattery(void);
static void App_Monitor_LogSysModeChange(void);
static void App_Monitor_UpdateModeByCurrent(void);
static void App_Monitor_UpdateSysMode(void);
static void App_Monitor_UpdateCellVoltage(void);
static void App_Monitor_UpdateBatteryVoltage(void);
static void App_Monitor_UpdateBatteryCurrent(void);
static void App_Monitor_UpdateCellTemperature(void);

void App_Monitor_Init(void)
{
	rt_thread_t p_thread;
	
	p_thread = rt_thread_create("monitor", 
							   App_Monitor_TaskEntry, 
							   NULL,
							   MONITOR_TASK_STACK_SIZE,
							   MONITOR_TASK_PRIORITY,
							   MONITOR_TASK_TIMESLICE);

   if (p_thread == NULL)
   {
	   LOG_E("Create Task Fail");
   }

	rt_thread_startup(p_thread);
}

static void App_Monitor_TaskEntry(void *parameter)
{
	while (1)
	{
		App_Monitor_SampleBattery();
		App_Monitor_UpdateSysMode();
		rt_thread_mdelay(MONITOR_TASK_PERIOD);
	}
}

// 监控电池各项数据
static void App_Monitor_SampleBattery(void)
{
	// 单体电芯电压
	s_cell_voltage_elapsed_ms += MONITOR_TASK_PERIOD;
	if (s_is_cell_voltage_enabled == true && s_cell_voltage_elapsed_ms >= UPDATE_CELL_VOLTAGE_PERIOD_MS)
	{
		App_Monitor_UpdateCellVoltage();
		s_cell_voltage_elapsed_ms = 0;
	}
	else if (s_is_cell_voltage_enabled == false)
	{
		s_cell_voltage_elapsed_ms = 0;
	}
	
	// 电池组电压
	s_battery_voltage_elapsed_ms += MONITOR_TASK_PERIOD;
	if (s_is_battery_voltage_enabled == true && s_battery_voltage_elapsed_ms >= UPDATE_BATTERY_VOLTAGE_PERIOD_MS)
	{
		App_Monitor_UpdateBatteryVoltage();
		s_battery_voltage_elapsed_ms = 0;		
	}
	else if (s_is_battery_voltage_enabled == false)
	{
		s_battery_voltage_elapsed_ms = 0;
	}

	// 电池温度
	s_cell_temp_elapsed_ms += MONITOR_TASK_PERIOD;
	if (s_is_cell_temp_enabled == true && s_cell_temp_elapsed_ms >= UPDATE_CELL_TEMP_PERIOD_MS)
	{
		App_Monitor_UpdateCellTemperature();
		s_cell_temp_elapsed_ms = 0;
	}
	else if (s_is_cell_temp_enabled == false)
	{
		s_cell_temp_elapsed_ms = 0;
	}

	// 电流采样由 AFE 的 CC_READY 中断置位(见 App_Monitor_OnCcSampleReady),这里只负责取走结果
	// 注意:该方式依赖 ALERT 引脚能否产生新的上升沿,所以每次烧写后必须让 AFE 复位一次
	//      (复位动作在 Int_BQ769X0_Init 里完成),否则中断不再触发
	if (s_is_cc_sample_ready == true && s_is_battery_current_enabled == true)
	{
		App_Monitor_UpdateBatteryCurrent();
		s_is_cc_sample_ready = false;
	}
}

// 系统模式发生变化时打印日志并复位待机计时
static void App_Monitor_LogSysModeChange(void)
{
	if (s_prev_sys_mode == g_st_global_param.sys_mode)
	{
		return;
	}

	switch (g_st_global_param.sys_mode)
	{
		case BMS_MODE_SLEEP:
			LOG_I("system entry sleep mode");
			break;
		case BMS_MODE_STANDBY:
			LOG_I("system entry standby mode");
			break;
		case BMS_MODE_CHARGE:
			LOG_I("system entry charge mode");
			break;
		case BMS_MODE_DISCHARGE:
			LOG_I("system entry discharge mode");
			break;
		case BMS_MODE_NULL:
			break;
	}

	s_standby_elapsed_ms = 0;
	s_prev_sys_mode = g_st_global_param.sys_mode;
}

// 依据电池电流判定并更新系统模式
//   |电流| <  MONITOR_STANDBY_CURRENT_*_A 围成的区间 : 待机(进而按时间进入睡眠)模式
//   电流 >= MONITOR_STANDBY_CURRENT_POS_A           : 充电模式
//   电流 <= MONITOR_STANDBY_CURRENT_NEG_A           : 放电模式
// 该阈值按实测的系统静态功耗确定:取值过小会把静态电流误判成充放电
static void App_Monitor_UpdateModeByCurrent(void)
{
	switch (g_st_global_param.sys_mode)
	{
		case BMS_MODE_SLEEP:
			if (g_st_monitor_data.battery_current >= MONITOR_STANDBY_CURRENT_POS_A || g_st_monitor_data.battery_current <= MONITOR_STANDBY_CURRENT_NEG_A)
			{
				// 可以加唤醒处理逻辑
				if (g_st_monitor_data.battery_current >= MONITOR_STANDBY_CURRENT_POS_A)
				{
					g_st_global_param.sys_mode = BMS_MODE_CHARGE;
				}
				else if (g_st_monitor_data.battery_current <= MONITOR_STANDBY_CURRENT_NEG_A)
				{
					g_st_global_param.sys_mode = BMS_MODE_DISCHARGE;
				}
			}
			break;

		case BMS_MODE_STANDBY:
			if (g_st_monitor_data.battery_current >= MONITOR_STANDBY_CURRENT_POS_A || g_st_monitor_data.battery_current <= MONITOR_STANDBY_CURRENT_NEG_A)
			{
				if (g_st_monitor_data.battery_current >= MONITOR_STANDBY_CURRENT_POS_A)
				{
					g_st_global_param.sys_mode = BMS_MODE_CHARGE;
				}
				else if (g_st_monitor_data.battery_current <= MONITOR_STANDBY_CURRENT_NEG_A)
				{
					g_st_global_param.sys_mode = BMS_MODE_DISCHARGE;
				}
			}
			s_standby_elapsed_ms += MONITOR_TASK_PERIOD;
			if (s_standby_elapsed_ms >= MONITOR_MIN_TO_MS(BMS_ENTRY_SLEEP_TIME))
			{
				if (g_st_energy_data.balance_release_flag != true)
				{
					// 可以加睡眠低功耗处理逻辑
					g_st_global_param.sys_mode = BMS_MODE_SLEEP;
				}
			}
			break;

		case BMS_MODE_CHARGE:
			if (g_st_monitor_data.battery_current < MONITOR_STANDBY_CURRENT_POS_A)
			{
				if (g_st_monitor_data.battery_current > MONITOR_STANDBY_CURRENT_NEG_A)
				{
					g_st_global_param.sys_mode = BMS_MODE_STANDBY;
				}
				else if (g_st_monitor_data.battery_current <= MONITOR_STANDBY_CURRENT_NEG_A)
				{
					g_st_global_param.sys_mode = BMS_MODE_DISCHARGE;
				}
			}
			break;

		case BMS_MODE_DISCHARGE:
			if (g_st_monitor_data.battery_current > MONITOR_STANDBY_CURRENT_NEG_A)
			{
				if (g_st_monitor_data.battery_current < MONITOR_STANDBY_CURRENT_POS_A)
				{
					g_st_global_param.sys_mode = BMS_MODE_STANDBY;
				}
				else if (g_st_monitor_data.battery_current >= MONITOR_STANDBY_CURRENT_POS_A)
				{
					g_st_global_param.sys_mode = BMS_MODE_CHARGE;
				}
			}
			break;

		case BMS_MODE_NULL:
			break;
	}
}

// 系统模式监控入口
static void App_Monitor_UpdateSysMode(void)
{
	App_Monitor_LogSysModeChange();
	App_Monitor_UpdateModeByCurrent();
}
void App_Monitor_SetCellVoltageEnable(BmsStateType new_state)
{
	if (new_state == BMS_STATE_ENABLE)
	{
		s_is_cell_voltage_enabled = true;
	}
	else if (new_state == BMS_STATE_DISABLE)
	{
		s_is_cell_voltage_enabled = false;
	}
}

void App_Monitor_SetBatteryVoltageEnable(BmsStateType new_state)
{
	if (new_state == BMS_STATE_ENABLE)
	{
		s_is_battery_voltage_enabled = true;
	}
	else if (new_state == BMS_STATE_DISABLE)
	{
		s_is_battery_voltage_enabled = false;
	}
}

void App_Monitor_SetCellTemperatureEnable(BmsStateType new_state)
{
	if (new_state == BMS_STATE_ENABLE)
	{
		s_is_cell_temp_enabled = true;
	}
	else if (new_state == BMS_STATE_DISABLE)
	{
		s_is_cell_temp_enabled = false;
	}	
}

void App_Monitor_SetBatteryCurrentEnable(BmsStateType new_state)
{
	if (new_state == BMS_STATE_ENABLE)
	{
		s_is_battery_current_enabled = true;
	}
	else if (new_state == BMS_STATE_DISABLE)
	{
		s_is_battery_current_enabled = false;
	}	
}

void App_Monitor_OnCcSampleReady(void)
{
	s_is_cc_sample_ready = true;
}


// 对电芯电压进行排序的比较函数
static int App_Monitor_CompareCell(void *p_e1, void *p_e2)
{
	float temp1 = (*(BmsCellData_Struct *)p_e1).cell_voltage;
	float temp2 = (*(BmsCellData_Struct *)p_e2).cell_voltage;

	if (temp1 > temp2)
	{
		return 1;
	}

	return 0;
}

static void App_Monitor_UpdateCellVoltage(void)
{
	uint8_t index;

	Int_BQ769X0_UpdateCellVoltage();
	for (index = 0; index < g_st_global_param.cell_real_number; index++)
	{
		g_st_monitor_data.cell_voltage_by_index[index] = g_st_bq769x0_sample_data.cell_voltage[index];
		g_st_monitor_data.cell_data_sorted[index].cell_voltage = g_st_bq769x0_sample_data.cell_voltage[index];
		g_st_monitor_data.cell_data_sorted[index].cell_index = index;
	}

	Com_Utils_BubbleSort(g_st_monitor_data.cell_data_sorted, g_st_global_param.cell_real_number, sizeof(BmsCellData_Struct), App_Monitor_CompareCell);
}

static void App_Monitor_UpdateBatteryVoltage(void)
{
	Int_BQ769X0_UpdateBatteryVoltage();
	g_st_monitor_data.battery_voltage = g_st_bq769x0_sample_data.battery_voltage;
}

static void App_Monitor_UpdateBatteryCurrent(void)
{
	Int_BQ769X0_UpdateCurrent();
	g_st_monitor_data.battery_current = g_st_bq769x0_sample_data.battery_current;
}

static void App_Monitor_UpdateCellTemperature(void)
{
	uint8_t index1 = 0, index2 = 0;

	Int_BQ769X0_UpdateTemperature();
	for (; index1 < g_st_global_param.temp_real_number; index1++)
	{
		if (g_st_bq769x0_sample_data.tsx_temperature[index1] >= BMS_TEMP_MEASURE_MIN &&
			g_st_bq769x0_sample_data.tsx_temperature[index1] <= BMS_TEMP_MEASURE_MAX)
		{
			g_st_monitor_data.cell_temp[index2++] = g_st_bq769x0_sample_data.tsx_temperature[index1];
		}
	}

	g_st_monitor_data.cell_temp_effective_number = index2;
	Com_Utils_BubbleSortFloat(g_st_monitor_data.cell_temp, index2);
}
