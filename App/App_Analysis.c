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
 * 模块职责:电池状态分析实现(SOC、容量、电压与功率统计)
 */
#include <stdio.h>
#include <stdlib.h>
#include <rtthread.h>

#include "App_Analysis.h"
#include "App_Config.h"

#include "App_Monitor.h"
#include "App_Protect.h"

#include "Com_Utils.h"
#include "App_Global.h"

#define DBG_TAG "analysis"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"

// p_thread config
#define ANALYSIS_TASK_STACK_SIZE	256
#define ANALYSIS_TASK_PRIORITY		21
#define ANALYSIS_TASK_TIMESLICE	25

#define ANALYSIS_TASK_PERIOD		1000

// 一小时包含的秒数
#define ANALYSIS_SEC_PER_HOUR	3600

#define TEMP_CAP_RATE_LIMIT_HIGH   1050
#define TEMP_CAP_RATE_LIMIT_LOW    750

BmsAnalysisData_Struct g_st_analysis_data =
{
	.capacity_rated = BMS_BATTERY_CAPACITY,
};

// 三元锂电池 soc 开路电压法计算数据表
static uint16_t s_soc_ocv_tab[101] =
{
	3282, // 0%~1%	
	3309, 3334, 3357, 3378, 3398, 3417, 3434, 3449, 3464, 3477,	// 0%~10%
	3489, 3500, 3510, 3520, 3528, 3536, 3543, 3549, 3555, 3561,	// 11%~20%
	3566, 3571, 3575, 3579, 3583, 3586, 3590, 3593, 3596, 3599,	// 21%~30%
	3602, 3605, 3608, 3611, 3615, 3618, 3621, 3624, 3628, 3632,	// 31%~40%
	3636, 3640, 3644, 3648, 3653, 3658, 3663, 3668, 3674, 3679,	// 41%~50%
	3685, 3691, 3698, 3704, 3711, 3718, 3725, 3733, 3741, 3748,	// 51%~60%
	3756, 3765, 3773, 3782, 3791, 3800, 3809, 3818, 3827, 3837,	// 61%~70%
	3847, 3857, 3867, 3877, 3887, 3897, 3908, 3919, 3929, 3940,	// 71%~80%
	3951, 3962, 3973, 3985, 3996, 4008, 4019, 4031, 4043, 4055,	// 81%~90%
	4067, 4080, 4092, 4105, 4118, 4131, 4145, 4158, 4172, 4185,	// 91~100%
};

static void App_Analysis_TaskEntry(void *parameter);

static void App_Analysis_CalcBasicData(void);
static void App_Analysis_CalibrateCapacity(void);
static void App_Analysis_SocCheck(void);
static void App_Analysis_CapAndSocInit(void);


// 电池状态分析模块初始化
void App_Analysis_Init(void)
{
	rt_thread_t p_thread;

	p_thread = rt_thread_create("analysis",
							   App_Analysis_TaskEntry,
							   NULL,
							   ANALYSIS_TASK_STACK_SIZE,
							   ANALYSIS_TASK_PRIORITY,
							   ANALYSIS_TASK_TIMESLICE);

	if (p_thread == NULL)
	{
		LOG_E("Create Task Fail");
	}

	rt_thread_startup(p_thread);
}

// 电池状态分析任务线程入口
static void App_Analysis_TaskEntry(void *parameter)
{
	App_Analysis_CapAndSocInit();
	while(1)
	{			
		App_Analysis_CalcBasicData();
		App_Analysis_CalibrateCapacity();
		App_Analysis_SocCheck();
		rt_thread_mdelay(ANALYSIS_TASK_PERIOD);
	}
}

// 简单分析,通过数据直接进行计算就能得到的
static void App_Analysis_CalcBasicData(void)
{
	uint8_t index;

	// 最大电压差
	g_st_analysis_data.max_voltage_difference = g_st_monitor_data.cell_data_sorted[g_st_global_param.cell_real_number - 1].cell_voltage - g_st_monitor_data.cell_data_sorted[0].cell_voltage;
	
	// 平均电压
	for (index = 0, g_st_analysis_data.average_voltage = 0; index < g_st_global_param.cell_real_number; index++)
	{
		g_st_analysis_data.average_voltage += g_st_monitor_data.cell_voltage_by_index[index];
	}
	g_st_analysis_data.average_voltage /= g_st_global_param.cell_real_number;
	
	// 实时功率
	g_st_analysis_data.power_real = g_st_monitor_data.battery_voltage * g_st_monitor_data.battery_current;	

	// 最大和最小电压
	g_st_analysis_data.cell_volt_max = g_st_monitor_data.cell_data_sorted[g_st_global_param.cell_real_number - 1].cell_voltage;
	g_st_analysis_data.cell_volt_min = g_st_monitor_data.cell_data_sorted[0].cell_voltage;
}

// 根据最小温度确定每摄氏度的容量校准倍率
static uint8_t App_Analysis_GetTempRatio(int16_t min_temp_x10)
{
	// 该校准倍率来自三元锂电池不同温度下的放电曲线,以25度常温为标准
	if (min_temp_x10 >= 250)
	{
		// 高于常温,每1度倍率0.001,容量增加
		return 1;
	}
	else if (min_temp_x10 >= 100)
	{
		// 低于常温,每1度倍率0.002,容量减小
		return 2;
	}
	else if (min_temp_x10 >= 0)
	{
		return 3;
	}
	else if (min_temp_x10 >= -200)
	{
		return 4;
	}
	else if (min_temp_x10 >= -300)
	{
		return 5;
	}

	// 温度越低,温度区间范围越大,变化趋势越大
	return 6;
}

// 温度校准
// 锂电池充放电时温度的变化会影响充放电时电压与时间的关系,进而影响电池实时容量
static void App_Analysis_CalibrateCapacityByTemp(void)
{
	static int16_t s_last_cal_temp_x10 = 0;

	uint8_t temp_cap_ratio;
	uint16_t capacity_rate;
	int16_t min_temp_x10;

	if (g_st_monitor_data.cell_temp_effective_number == 0)
	{
		return;
	}

	// 小数转化成整数,方便计算
	min_temp_x10 = g_st_monitor_data.cell_temp[0] * 10;

	// 温度变化不足1度则不执行校准
	if (min_temp_x10 > s_last_cal_temp_x10)
	{
		if (min_temp_x10 - s_last_cal_temp_x10 < 10)
		{
			return;
		}
	}
	else if (s_last_cal_temp_x10 - min_temp_x10 < 10)
	{
		return;
	}

	s_last_cal_temp_x10 = min_temp_x10;

	temp_cap_ratio = App_Analysis_GetTempRatio(min_temp_x10);

	// 1000 表示容量为100%,temp_cap_ratio 表示每摄氏度容量衰减/增加的倍率
	// (min_temp_x10 - 250) / 10 表示偏离常温的度数,capacity_rate 即容量百分比
	capacity_rate = 1000 + temp_cap_ratio * (min_temp_x10 - 250) / 10;

	// 上下限:105% / 75%
	if (capacity_rate > TEMP_CAP_RATE_LIMIT_HIGH)
	{
		capacity_rate = TEMP_CAP_RATE_LIMIT_HIGH;
	}
	else if (capacity_rate < TEMP_CAP_RATE_LIMIT_LOW)
	{
		capacity_rate = TEMP_CAP_RATE_LIMIT_LOW;
	}

	// 实时容量
	g_st_analysis_data.capacity_real = g_st_analysis_data.capacity_rated * capacity_rate / 1000;

	// 剩余容量
	g_st_analysis_data.capacity_remain = g_st_analysis_data.capacity_real * g_st_analysis_data.soc;
}
// 实时校准容量涉及因素:温度、完整充放电、老化等等
static void App_Analysis_CalibrateCapacity(void)
{	
	App_Analysis_CalibrateCapacityByTemp();
}

// 根据单体电芯最低电压计算出soc值,用于上电和长时间静止状态下的校准
static uint16_t App_Analysis_OcvToSoc(uint16_t voltage)
{
	uint16_t soc = 0;
	
	if (voltage <= s_soc_ocv_tab[0])
	{
		soc  = 0;
	}
	else if (voltage >= s_soc_ocv_tab[100])
	{
		soc = 1000;
	}
	else
	{
		uint16_t index = Com_Utils_FirstNotLess(s_soc_ocv_tab, 0, 100, voltage);

		if (voltage == s_soc_ocv_tab[index])
		{
			// 整数SOC值
			soc = index * 10;
		}
		else
		{
			// (index - 1) * 10计算整数位，后面一坨计算小数点
			soc = (index - 1) * 10 + ((voltage - s_soc_ocv_tab[index - 1]) * 10) / ((s_soc_ocv_tab[index] - s_soc_ocv_tab[index - 1]));			
		}
	}
	
	return soc;
}

// 开路电压法soc计算
static void App_Analysis_OcvSocCalculate(void)
{
	// 进入睡眠的条件:待机一段时间以上且没有电池在均衡
	if (g_st_global_param.sys_mode == BMS_MODE_SLEEP)
	{
		// 等待一段时间电压平稳,防止均衡才刚结束
		rt_thread_mdelay(BALANCE_VOLT_RISE_DELAY);

		// 开路电压校准
		g_st_analysis_data.soc = App_Analysis_OcvToSoc(g_st_monitor_data.cell_data_sorted[0].cell_voltage  * 1000) / 1000.0;

		// 剩余容量 = 实际容量 * soc
		g_st_analysis_data.capacity_remain = g_st_analysis_data.capacity_real * g_st_analysis_data.soc;		
	}
}

// 安时积分法soc计算
// 待机模式下判断最低电压值是否大于等于过压保护值,成立则soc = 100%
// 待机模式下判断最低电压值是否小于等于欠压保护值,成立则soc = 0%
// 充电时对进行测量出来的电流值+积分
// 放电时对进行测量出来的电流值-积分
// soc = 实时积分的容量 / 电池包实际容量
static void App_Analysis_AhSocCalculate(void)
{
	// abs取绝对值，除3600把 AS 单位换算成 Ah
	// 这里为什么要*1000然后再除1000，因为abs只能对整数取绝对值所以要将小数转整数，这里也可以用fabs就不用*1000/1000操作了
	float current_value = abs((int32_t)(g_st_monitor_data.battery_current * 1000)) / 1000.0 / ANALYSIS_SEC_PER_HOUR;

	if (g_st_global_param.sys_mode == BMS_MODE_STANDBY)
	{
		if (g_st_monitor_data.cell_data_sorted[0].cell_voltage >= g_st_protect.param.ov_protect)
		{
			g_st_analysis_data.soc = 1;
		}
		else if (g_st_monitor_data.cell_data_sorted[0].cell_voltage <= g_st_protect.param.uv_protect)
		{
			g_st_analysis_data.soc = 0;
		}
	}

	if (g_st_global_param.sys_mode == BMS_MODE_CHARGE)
	{
		if(g_st_analysis_data.capacity_real >= (g_st_analysis_data.capacity_remain + current_value))
		{
			g_st_analysis_data.capacity_remain += current_value;
		}
		else
		{
			g_st_analysis_data.capacity_remain = g_st_analysis_data.capacity_real;
		}
	}
	else if (g_st_global_param.sys_mode == BMS_MODE_DISCHARGE)
	{
		if(g_st_analysis_data.capacity_remain >= current_value)
		{
			g_st_analysis_data.capacity_remain -= current_value;
		}
		else
		{		
			g_st_analysis_data.capacity_remain = 0;
		}
	}

	/*
	else  // 是否考虑静态时的漏电电流10mA
	{
		if(g_st_analysis_data.capacity_remain >= 0.01)   
		{
		 	g_st_analysis_data.capacity_remain -= 0.01;
		}
		else
		{
		 	g_st_analysis_data.capacity_remain = 0;
		}
	}
	*/

	g_st_analysis_data.soc = g_st_analysis_data.capacity_remain / g_st_analysis_data.capacity_real;
	if (g_st_analysis_data.soc > 1)
	{
		g_st_analysis_data.soc = 1;
	}
}

// soc检查
static void App_Analysis_SocCheck(void)
{
	App_Analysis_OcvSocCalculate();
	App_Analysis_AhSocCalculate();
}

// 容量和SOC上电初始化
static void App_Analysis_CapAndSocInit(void)
{
	// soc计算
	g_st_analysis_data.soc = App_Analysis_OcvToSoc(g_st_monitor_data.cell_data_sorted[0].cell_voltage  * 1000) / 1000.0;

	// 实际容量后面再完善,涉及到完整充放电流计算、老化损耗、温度特性曲线、信息存储模块
	g_st_analysis_data.capacity_real = g_st_analysis_data.capacity_rated;

	// 剩余容量 = 实际容量 * soc
	g_st_analysis_data.capacity_remain = g_st_analysis_data.capacity_real * g_st_analysis_data.soc; 
}
