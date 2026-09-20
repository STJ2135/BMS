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
 * 模块职责:信息输出实现(容量指示灯 + 参数打印)
 */
#include <stdio.h>
#include <rtthread.h>

#include "App_Info.h"

#include "Int_LED.h"

#include "App_Monitor.h"
#include "App_Energy.h"
#include "App_Analysis.h"
#include "App_Global.h"

#define DBG_TAG "info"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"

// thread config
#define INFO_TASK_STACK_SIZE 512
#define INFO_TASK_PRIORITY 25
#define INFO_TASK_TIMESLICE 25

#define INFO_TASK_PERIOD 2000

// SOC 指示灯分档阈值
#define INFO_LED_SOC_LEVEL1 0.25F
#define INFO_LED_SOC_LEVEL2 0.50F
#define INFO_LED_SOC_LEVEL3 0.75F

static bool s_is_info_print_enabled = true;

static void App_Info_TaskEntry(void *parameter);

static void App_Info_UpdateCapacityIndicator(void);
static void App_Info_PrintCapacity(void);
static void App_Info_PrintAnalysis(void);
static void App_Info_PrintSample(void);

void App_Info_Init(void)
{
	rt_thread_t p_thread;

	p_thread = rt_thread_create("info",
							  App_Info_TaskEntry,
							  NULL,
							  INFO_TASK_STACK_SIZE,
							  INFO_TASK_PRIORITY,
							  INFO_TASK_TIMESLICE);

	if (p_thread == NULL)
	{
		LOG_E("Create Task Fail");
	}

	rt_thread_startup(p_thread);
}

static void App_Info_TaskEntry(void *parameter)
{
	while (1)
	{
		if (s_is_info_print_enabled == true)
		{
			App_Info_Print();
		}

		App_Info_UpdateCapacityIndicator();
		rt_thread_mdelay(INFO_TASK_PERIOD);
	}
}

// 电池容量指示灯
static void App_Info_UpdateCapacityIndicator(void)
{
	uint8_t index;
	uint8_t light_count = 0;

	if (g_st_analysis_data.soc > INFO_LED_SOC_LEVEL3)
	{
		light_count = 0;
	}
	else if (g_st_analysis_data.soc > INFO_LED_SOC_LEVEL2)
	{
		light_count = 1;
	}
	else if (g_st_analysis_data.soc > INFO_LED_SOC_LEVEL1)
	{
		light_count = 2;
	}
	else if (g_st_analysis_data.soc > 0.0F)
	{
		light_count = 3;
	}
	else
	{
		light_count = 4;
	}

	for (index = 0; index < INT_LED_INDEX_MAX; index++)
	{
		Int_LED_SetState((IntLedIndexType)index, index < light_count);
	}
}

// 打印容量与 SOC
static void App_Info_PrintCapacity(void)
{
	char str[64];

	sprintf(str, "Battery Real Capacity = %0.3fAh", g_st_analysis_data.capacity_real);
	LOG_D("%s", str);

	sprintf(str, "Battery Remain Capacity = %0.3fAh", g_st_analysis_data.capacity_remain);
	LOG_D("%s", str);

	rt_kprintf("\r\n");

	sprintf(str, "Battery SOC = %0.1f%%", g_st_analysis_data.soc * 100);
	LOG_D("%s", str);

	rt_kprintf("\r\n");
}

// 打印电池状态分析结果
static void App_Info_PrintAnalysis(void)
{
	char str[64];

	sprintf(str, "Cell Max Voltage = %0.3fV", g_st_analysis_data.cell_volt_max);
	LOG_D("%s", str);

	sprintf(str, "Cell Min Voltage = %0.3fV", g_st_analysis_data.cell_volt_min);
	LOG_D("%s", str);

	sprintf(str, "Cell Max Voltage Difference = %0.3fV", g_st_analysis_data.max_voltage_difference);
	LOG_D("%s", str);

	sprintf(str, "Cell Average Voltage = %0.3fV", g_st_analysis_data.average_voltage);
	LOG_D("%s", str);

	sprintf(str, "Battery Real Power = %0.3fW", g_st_analysis_data.power_real);
	LOG_D("%s", str);
}

// 打印实时采样数据
static void App_Info_PrintSample(void)
{
	uint8_t index;
	char str[64];

	rt_kprintf("\r\n");

	sprintf(str, "Battery Voltage = %0.3fV", g_st_monitor_data.battery_voltage);
	LOG_D("%s", str);

	sprintf(str, "Battery Current = %0.3fA", g_st_monitor_data.battery_current);
	LOG_D("%s", str);

	for (index = 0; index < g_st_monitor_data.cell_temp_effective_number; index++)
	{
		sprintf(str, "Temperature %d = %0.1f", index + 1, g_st_monitor_data.cell_temp[index]);
		LOG_D("%s", str);
	}

	rt_kprintf("\r\n");

	for (index = 0; index < g_st_global_param.cell_real_number; index++)
	{
		sprintf(str, "Cell%-2d Voltage = %-5.3fV %s",
				index + 1,
				g_st_monitor_data.cell_voltage_by_index[index],
				(g_st_energy_data.balance_record & (1 << index)) > 0 ? "--->" : "");
		LOG_D("%s", str);
	}
}

// 实时打印BMS信息
void App_Info_Print(void)
{
	LOG_D("/*************************************************************/");

	App_Info_PrintCapacity();
	App_Info_PrintAnalysis();
	App_Info_PrintSample();

	LOG_D("/*************************************************************/\r\n\r\n");
}

void App_Info_SetPrintEnable(BmsStateType new_state)
{
	if (new_state == BMS_STATE_ENABLE)
	{
		s_is_info_print_enabled = true;
	}
	else if (new_state == BMS_STATE_DISABLE)
	{
		s_is_info_print_enabled = false;
	}
}
