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
 * 模块职责:能量管理实现(均衡筛选与启停 + 充放电启停)
 */
#include <stdio.h>
#include <stdbool.h>


#include "App_Energy.h"

#include "App_Power.h"
#include "App_Config.h"

#include "Int_BQ769X0.h"

#include "App_Monitor.h"
#include "App_Analysis.h"
#include "App_Protect.h"
#include "App_Global.h"


#define DBG_TAG "energy"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"


// p_thread config
#define ENERGY_TASK_STACK_SIZE	512
#define ENERGY_TASK_PRIORITY	22
#define ENERGY_TASK_TIMESLICE	25

#define ENERGY_TASK_PERIOD		200


BmsEnergyData_Struct g_st_energy_data = 
{
	.soc_stop_chg   	= SOC_STOP_CHG_VALUE,
	.soc_start_chg  	= SOC_START_CHG_VALUE,
	.soc_stop_dsg   	= SOC_STOP_DSG_VALUE,
	.soc_start_dsg  	= SOC_START_DSG_VALUE,

	.balance_start_voltage = INIT_BALANCE_VOLTAGE,
	.balance_diff_voltage = BALANCE_DIFF_VOLTAGE,
	.balance_cycle_time 	 = BALANCE_CYCLE_TIME,
	.balance_record 		 = BMS_CELL_NONE,
};


static rt_timer_t s_p_balance_timer;
static bool s_is_balancing = false;
static uint32_t s_next_balance_tick;


static void App_Energy_TaskEntry(void *parameter);
static void App_Energy_BalanceTimerEntry(void *parameter);

static void App_Energy_ManageByMode(void);
static bool App_Energy_IsCellBalanceAllowed(uint8_t cell_index);
static void App_Energy_ManageChargeDischarge(void);
static void App_Energy_BalanceManage(void);


void App_Energy_Init(void)
{
	rt_thread_t p_thread;

	
	p_thread = rt_thread_create("energy",
							   App_Energy_TaskEntry,
							   NULL,
							   ENERGY_TASK_STACK_SIZE,
							   ENERGY_TASK_PRIORITY,
							   ENERGY_TASK_TIMESLICE);

	if (p_thread == NULL)
	{
		LOG_E("Create Task Fail");
	}

	rt_thread_startup(p_thread);


	s_p_balance_timer = rt_timer_create("balance", 
									App_Energy_BalanceTimerEntry,
									NULL,
									20,
									RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);

	if (s_p_balance_timer == NULL)
	{
		LOG_E("Create Timer Fail");
	}
}


// 电池能量管理任务线程入口
static void App_Energy_TaskEntry(void *parameter)
{
	rt_thread_mdelay(500);
	
	while(1)
	{
		App_Energy_BalanceManage();
		App_Energy_ManageChargeDischarge();
		rt_thread_mdelay(ENERGY_TASK_PERIOD);
	}
}


// 充放电管理
static void App_Energy_ManageChargeDischarge(void)
{
	App_Energy_ManageByMode();
	App_Power_ApplyCommandChange();
}

// 按当前系统模式管理充放电启停
static void App_Energy_ManageByMode(void)
{
	switch(g_st_global_param.sys_mode)
	{
		case BMS_MODE_CHARGE:
		{
			if (g_st_analysis_data.soc >= g_st_energy_data.soc_stop_chg)
			{
				App_Power_RequestCharge(BMS_STATE_DISABLE);

				LOG_I("Stop Charge");
			}
		}break;

		case BMS_MODE_DISCHARGE:
		{
			if (g_st_analysis_data.soc <= g_st_energy_data.soc_stop_dsg)
			{
				App_Power_RequestDischarge(BMS_STATE_DISABLE);

				LOG_I("Stop Discharge");
			}
		}break;

		case BMS_MODE_STANDBY:
		{
			if (g_st_global_param.charge_allowed == BMS_STATE_ENABLE)
			{
				if ((App_Protect_GetAlert() & FLAG_ALERT_CHG_MASK) == FLAG_ALERT_NO)
				{
					if (g_st_energy_data.balance_release_flag != true)
					{
						if (g_st_analysis_data.soc < g_st_energy_data.soc_start_chg)
						{
							App_Power_RequestCharge(BMS_STATE_ENABLE);

							LOG_I("Start Charge");
						}
					}
				}
			}

			if (g_st_global_param.discharge_allowed == BMS_STATE_ENABLE)
			{
				if ((App_Protect_GetAlert() & FLAG_ALERT_DSG_MASK) == FLAG_ALERT_NO)
				{
					if (g_st_analysis_data.soc > g_st_energy_data.soc_start_dsg)
					{
						App_Power_RequestDischarge(BMS_STATE_ENABLE);

						LOG_I("Start Discharge");
					}
				}
			}
		}break;
		default:;break;
	}
}


// 用于均衡计数的定时器回调入口
static void App_Energy_BalanceTimerEntry(void *parameter)
{
	(void)parameter;

	Int_BQ769X0_SetCellsBalance(BMS_CELL_ALL, BMS_STATE_DISABLE);

	g_st_energy_data.balance_record = BMS_CELL_NONE;
	
	s_is_balancing = false;

	// 用于均衡电压回升计时
	s_next_balance_tick = rt_tick_from_millisecond(BALANCE_VOLT_RISE_DELAY) + rt_tick_get();
	
	LOG_I("Balance Timer End");
}

// 启动均衡定时器计数任务
static void App_Energy_BalanceStartTimer(uint32_t sec)
{
	uint32_t tick;

	tick = rt_tick_from_millisecond(sec * 1000);
	rt_timer_control(s_p_balance_timer, RT_TIMER_CTRL_SET_TIME, &tick);
	rt_timer_start(s_p_balance_timer);

	LOG_I("Balance Timer Start");
}


// 均衡启动条件检查
static bool App_Energy_BalanceCheck(void)
{
	// 上一轮均衡时间等待还未结束
	if (s_next_balance_tick >= rt_tick_get())
	{
		return false;
	}

	// 均衡定时器启动
	if (s_is_balancing != false)
	{
		return false;
	}

	// 未使能均衡
	if (g_st_global_param.balance_allowed != BMS_STATE_ENABLE)
	{
		g_st_energy_data.balance_release_flag = false;
		
		return false;
	}

	// 未处于待机和充电模式
	if (g_st_global_param.sys_mode != BMS_MODE_STANDBY && g_st_global_param.sys_mode != BMS_MODE_CHARGE)		
	{
		g_st_energy_data.balance_release_flag = false;
		
		return false;				
	}
	
	// 最高电池电压小于均衡起始电压
	if (g_st_monitor_data.cell_data_sorted[g_st_global_param.cell_real_number-1].cell_voltage < g_st_energy_data.balance_start_voltage)
	{
		g_st_energy_data.balance_release_flag = false;
		
		return false;				
	}

	// 最高和最低电池的电压差未达到均衡条件
	if (g_st_analysis_data.max_voltage_difference < g_st_energy_data.balance_diff_voltage)
	{	
		g_st_energy_data.balance_release_flag = false;
		
		return false;
	}

	g_st_energy_data.balance_release_flag = true;

	return true;
}

// 判断该节电芯是否允许加入均衡(相邻电芯不能同时均衡)
static bool App_Energy_IsCellBalanceAllowed(uint8_t cell_index)
{
	if (cell_index == 0)
	{
		// 第一节电芯,判断第二节是否已加入均衡
		return (g_st_energy_data.balance_record & 0x02) == 0;
	}
	else if (cell_index + 1 == g_st_global_param.cell_real_number)
	{
		// 最后一节电芯,判断前一节是否已加入均衡
		return (g_st_energy_data.balance_record & (1 << (cell_index - 1))) == 0;
	}

	// 其他电芯,判断前后相邻电芯是否已加入均衡
	return ((g_st_energy_data.balance_record & (1 << (cell_index - 1))) == 0) &&
		   ((g_st_energy_data.balance_record & (1 << (cell_index + 1))) == 0);
}

// 均衡电池筛选(相邻单元不能同时均衡,按电压从大到小筛选)
static void App_Energy_BalanceFilter(void)
{
	uint8_t index;
	float cmp_voltage;
	float min_voltage = g_st_monitor_data.cell_data_sorted[0].cell_voltage;

	for (index = 1; index < g_st_global_param.cell_real_number + 1; index++)
	{
		uint8_t cell_index;

		cmp_voltage = g_st_monitor_data.cell_data_sorted[g_st_global_param.cell_real_number - index].cell_voltage;

		if (cmp_voltage - min_voltage <= g_st_energy_data.balance_diff_voltage)
		{
			break;
		}

		cell_index = g_st_monitor_data.cell_data_sorted[g_st_global_param.cell_real_number - index].cell_index;

		if (App_Energy_IsCellBalanceAllowed(cell_index) == true)
		{
			LOG_I("Balance Cell:%d", cell_index + 1);
			g_st_energy_data.balance_record |= 1 << cell_index;
		}
	}
}
// 均衡启动
static void App_Energy_BalanceStart(void)
{
	if (g_st_energy_data.balance_record != BMS_CELL_NONE)
	{
		// 操作实际硬件
		Int_BQ769X0_SetCellsBalance(g_st_energy_data.balance_record, BMS_STATE_ENABLE);
		App_Energy_BalanceStartTimer(g_st_energy_data.balance_cycle_time);
	
		s_is_balancing = true;
		
		LOG_I("Balance Start");
	}
}


// 均衡管理
static void App_Energy_BalanceManage(void)
{
	if (App_Energy_BalanceCheck() == true)
	{
		App_Energy_BalanceFilter();
		App_Energy_BalanceStart();
	}
}


