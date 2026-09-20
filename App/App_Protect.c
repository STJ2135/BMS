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
 * 模块职责:电池保护实现(软件保护检测与解除 + AFE 硬件报警回调)
 */
#include <stdio.h>
#include <stdbool.h>
#include <rtthread.h>
#include <rthw.h>

#include "App_Protect.h"
#include "App_Config.h"
#include "App_Power.h"

#include "Int_BQ769X0.h"
#include "App_Monitor.h"
#include "App_Global.h"


#define DBG_TAG "protect"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"

// p_thread config
#define PROTECT_TASK_STACK_SIZE	256
#define PROTECT_TASK_PRIORITY	20
#define PROTECT_TASK_TIMESLICE	25

#define PROTECT_TASK_PERIOD		200

// 毫秒换算成秒
#define PROTECT_MS_PER_SEC		1000

// 过流/短路保护解除计时(ms)
static uint32_t s_charge_release_elapsed_ms = 0;
static uint32_t s_discharge_release_elapsed_ms = 0;
 
BmsProtect_Struct g_st_protect = 
{
	.alert = FLAG_ALERT_NO,
	.param = 
	{
		.shutdown_voltage = INIT_SHUTDOWN_VOLTAGE,

		.ov_protect	= INIT_OV_PROTECT,
		.ov_release	= INIT_OV_RELEASE,
		.uv_protect	= INIT_UV_PROTECT,
		.uv_release	= INIT_UV_RELEASE,

		.occ_protect = INIT_OCC_MAX,
		.ocd_protect = INIT_OCD_MAX,

		.ov_delay	= INIT_OV_DELAY,
		.uv_delay	= INIT_UV_DELAY,
		.ocd_delay	= INIT_OCD_DELAY,
		.scd_delay	= INIT_SCD_DELAY,

		.ocd_release = INIT_OCD_RELEASE,
		.scd_release = INIT_SCD_RELEASE,
		.occ_delay	= INIT_OCC_DELAY,
		.occ_release = INIT_OCC_RELEASE,

		.otc_protect = INIT_OTC_PROTECT,
		.otc_release = INIT_OTC_RELEASE,
		.otd_protect = INIT_OTD_PROTECT,
		.otd_release = INIT_OTD_RELEASE,

		.ltc_protect = INIT_LTC_PROTECT,
		.ltc_release = INIT_LTC_RELEASE,
		.ltd_protect = INIT_LTD_PROTECT,
		.ltd_release = INIT_LTD_RELEASE,
	}
};


static void App_Protect_AlertSet(BmsProtectAlertType flag);
static void App_Protect_AlertClear(BmsProtectAlertType flag);
static void App_Protect_TaskEntry(void *parameter);

// 保护报警标志会被硬件报警中断和保护线程同时修改,读写都需要进临界区
BmsProtectAlertType App_Protect_GetAlert(void)
{
	rt_base_t level = rt_hw_interrupt_disable();
	BmsProtectAlertType alert = g_st_protect.alert;
	rt_hw_interrupt_enable(level);

	return alert;
}

static void App_Protect_AlertSet(BmsProtectAlertType flag)
{
	rt_base_t level = rt_hw_interrupt_disable();
	g_st_protect.alert |= flag;
	rt_hw_interrupt_enable(level);
}

static void App_Protect_AlertClear(BmsProtectAlertType flag)
{
	rt_base_t level = rt_hw_interrupt_disable();
	g_st_protect.alert &= ~flag;
	rt_hw_interrupt_enable(level);
}


static void App_Protect_CheckSoftwareProtection(void);
static void App_Protect_CheckChargeRelease(void);
static void App_Protect_CheckDischargeRelease(void);
static void App_Protect_CheckProtectionRelease(void);

// 保护任务的初始化
void App_Protect_Init(void)
{
	rt_thread_t p_thread;

	
	p_thread = rt_thread_create("protect", 
							   App_Protect_TaskEntry, 
							   NULL,
							   PROTECT_TASK_STACK_SIZE,
							   PROTECT_TASK_PRIORITY,
							   PROTECT_TASK_TIMESLICE);

	if (p_thread == NULL)
	{
		LOG_E("Create Task Fail");
	}

	rt_thread_startup(p_thread);
}


// 保护任务入口
static void App_Protect_TaskEntry(void *parameter)
{
	(void)parameter;
	
	while(1)
	{
		App_Protect_CheckSoftwareProtection();
		App_Protect_CheckProtectionRelease();		
		rt_thread_mdelay(PROTECT_TASK_PERIOD);
	}
}









// 充电监控:过流、过温、低温
// 触发三种保护里面的一种就会关闭充电,直到达到恢复条件
static void App_Protect_CheckChargeProtection(void)
{
	static uint32_t s_occ_delay_elapsed_ms = 0;	// 充电过流已持续时间,达到 occ_delay 才触发保护

	if (g_st_monitor_data.battery_current > g_st_protect.param.occ_protect)
	{
		// 过流
		s_occ_delay_elapsed_ms += PROTECT_TASK_PERIOD;
		if (s_occ_delay_elapsed_ms / PROTECT_MS_PER_SEC >= g_st_protect.param.occ_delay)
		{			
			App_Power_LockCharge();
			App_Protect_AlertSet(FLAG_ALERT_OCC);

			LOG_W("Charge:OCC Protect Trigger");
		}
	}
	else if (g_st_monitor_data.cell_temp_effective_number == 0)
	{
		// 无效的温度不进行比较
		return;
	}
	else if (g_st_monitor_data.cell_temp[g_st_monitor_data.cell_temp_effective_number-1] > g_st_protect.param.otc_protect)
	{
		// 过温
		App_Power_LockCharge();
		App_Protect_AlertSet(FLAG_ALERT_OTC);	
		
		LOG_W("Charge:OTC Protect Trigger");
	}
	else if (g_st_monitor_data.cell_temp[0] < g_st_protect.param.ltc_protect)
	{
		// 低温
		App_Power_LockCharge();
		App_Protect_AlertSet(FLAG_ALERT_LTC);	

		LOG_W("Charge:LTC Protect Trigger");
	}
	else
	{
		// 复位计数
		s_occ_delay_elapsed_ms = 0;
	}
}

// 放电监控
static void App_Protect_CheckDischargeProtection(void)
{
	if (g_st_monitor_data.cell_temp_effective_number == 0)
	{
		// 无无效的温度不进行比较
		return;
	}
	else if (g_st_monitor_data.cell_temp[g_st_monitor_data.cell_temp_effective_number-1] > g_st_protect.param.otd_protect)
	{
		// 过温
		App_Power_LockDischarge();
		App_Protect_AlertSet(FLAG_ALERT_OTD);

		LOG_W("Discharge:OTD Protect Trigger");
	}
	else if (g_st_monitor_data.cell_temp[0] < g_st_protect.param.ltd_protect)
	{
		// 低温
		App_Power_LockDischarge();		
		App_Protect_AlertSet(FLAG_ALERT_LTD);	
		
		LOG_W("Discharge:LTD Protect Trigger");
	}
}


// 按当前系统模式执行"软件保护"检测(过流/过温/低温等)
// 说明:OCD/SCD/OV/UV 由 AFE 硬件比较器触发,走中断回调,不在这里处理
static void App_Protect_CheckSoftwareProtection(void)
{
	switch(g_st_global_param.sys_mode)
	{
		case BMS_MODE_CHARGE:
		{
			App_Protect_CheckChargeProtection();
		}break;

		case BMS_MODE_DISCHARGE:
		{
			App_Protect_CheckDischargeProtection();
		}break;

		case BMS_MODE_STANDBY:
		{
		}break;

		case BMS_MODE_SLEEP:
		{
			// 睡眠暂时没什么可监控的
		}break;
		
		default:;break;
	}
}


// 保护解除监控
static void App_Protect_CheckProtectionRelease(void)
{
	if (App_Protect_GetAlert() == FLAG_ALERT_NO)
	{
		return;
	}

	App_Protect_CheckChargeRelease();
	App_Protect_CheckDischargeRelease();
}

// 充电类报警解除检查
static void App_Protect_CheckChargeRelease(void)
{
	if (App_Protect_GetAlert() & FLAG_ALERT_OV)
	{
		if (g_st_monitor_data.cell_data_sorted[BMS_CELL_MAX - 1].cell_voltage < g_st_protect.param.ov_release)
		{
			App_Protect_AlertClear(FLAG_ALERT_OV);
			App_Power_UnlockCharge();

			LOG_I("Charge:OV Release");
		}
	}
	else if (App_Protect_GetAlert() & FLAG_ALERT_OTC)
	{
		if (g_st_monitor_data.cell_temp_effective_number > 0 &&
			    g_st_monitor_data.cell_temp[BMS_TEMP_MAX - 1] < g_st_protect.param.otc_release)
		{
			App_Protect_AlertClear(FLAG_ALERT_OTC);
			App_Power_ReleaseCharge();

			LOG_I("Charge:OTC Release");
		}
	}
	else if (App_Protect_GetAlert() & FLAG_ALERT_LTC)
	{
		if (g_st_monitor_data.cell_temp_effective_number > 0 &&
			    g_st_monitor_data.cell_temp[0] > g_st_protect.param.ltc_release)
		{
			App_Protect_AlertClear(FLAG_ALERT_LTC);
			App_Power_ReleaseCharge();

			LOG_I("Charge:LTC Release");
		}
	}
	else if (App_Protect_GetAlert() & FLAG_ALERT_OCC)
	{
		s_charge_release_elapsed_ms += PROTECT_TASK_PERIOD;
		if (s_charge_release_elapsed_ms / PROTECT_MS_PER_SEC >= g_st_protect.param.occ_release)
		{
			s_charge_release_elapsed_ms = 0;

			App_Protect_AlertClear(FLAG_ALERT_OCC);
			App_Power_ReleaseCharge();

			LOG_I("Charge:OCC Release");
		}
	}
}

// 放电类报警解除检查
static void App_Protect_CheckDischargeRelease(void)
{
	if (App_Protect_GetAlert() & FLAG_ALERT_UV)
	{
		if (g_st_monitor_data.cell_data_sorted[0].cell_voltage > g_st_protect.param.uv_release)
		{
			App_Protect_AlertClear(FLAG_ALERT_UV);
			App_Power_UnlockDischarge();

			LOG_I("Discharge:UV Release");
		}
	}
	else if (App_Protect_GetAlert() & FLAG_ALERT_OTD)
	{
		if (g_st_monitor_data.cell_temp_effective_number > 0 &&
			    g_st_monitor_data.cell_temp[BMS_TEMP_MAX - 1] < g_st_protect.param.otd_release)
		{
			App_Protect_AlertClear(FLAG_ALERT_OTD);
			App_Power_ReleaseDischarge();

			LOG_I("Discharge:OTD Release");
		}
	}
	else if (App_Protect_GetAlert() & FLAG_ALERT_LTD)
	{
		if (g_st_monitor_data.cell_temp_effective_number > 0 &&
			    g_st_monitor_data.cell_temp[0] > g_st_protect.param.ltd_release)
		{
			App_Protect_AlertClear(FLAG_ALERT_LTD);
			App_Power_ReleaseDischarge();

			LOG_I("Discharge:LTD Release");
		}
	}
	else if (App_Protect_GetAlert() & FLAG_ALERT_OCD)
	{
		s_discharge_release_elapsed_ms += PROTECT_TASK_PERIOD;
		if (s_discharge_release_elapsed_ms / PROTECT_MS_PER_SEC >= g_st_protect.param.ocd_release)
		{
			s_discharge_release_elapsed_ms = 0;

			App_Protect_AlertClear(FLAG_ALERT_OCD);
			App_Power_ReleaseDischarge();

			LOG_I("Discharge:OCD Release");
		}
	}
	else if (App_Protect_GetAlert() & FLAG_ALERT_SCD)
	{
		s_discharge_release_elapsed_ms += PROTECT_TASK_PERIOD;
		if (s_discharge_release_elapsed_ms / PROTECT_MS_PER_SEC >= g_st_protect.param.scd_release)
		{
			s_discharge_release_elapsed_ms = 0;

			App_Protect_AlertClear(FLAG_ALERT_SCD);
			App_Power_ReleaseDischarge();

			LOG_I("Discharge:SCD Release");
		}
	}
}

// 放电过流(OCD)硬件触发
void App_Protect_OnOcd(void)
{
	if ((App_Protect_GetAlert() & FLAG_ALERT_OCD) == FLAG_ALERT_NO) // 判断是为了防止多次触发
	{
		App_Power_LockDischarge();
		App_Protect_AlertSet(FLAG_ALERT_OCD);
		LOG_W("Discharge:OCD Protect Trigger");
	}
}

// 放电短路(SCD)硬件触发
void App_Protect_OnScd(void)
{
	if ((App_Protect_GetAlert() & FLAG_ALERT_SCD) == FLAG_ALERT_NO) // 判断是为了防止多次触发
	{
		App_Power_LockDischarge();
		App_Protect_AlertSet(FLAG_ALERT_SCD);
		LOG_W("Discharge:SCD Protect Trigger");
	}
}

// 充电过压(OV)硬件触发
void App_Protect_OnOv(void)
{
	if ((App_Protect_GetAlert() & FLAG_ALERT_OV) == FLAG_ALERT_NO) // 判断是为了防止多次触发
	{
		App_Power_LockCharge();
		App_Protect_AlertSet(FLAG_ALERT_OV);
		LOG_W("Charge:OV Protect Trigger");
	}
}

// 放欠过压(UV)硬件触发
void App_Protect_OnUv(void)
{
	if ((App_Protect_GetAlert() & FLAG_ALERT_UV) == FLAG_ALERT_NO) // 判断是为了防止多次触发
	{
		App_Power_LockDischarge();
		App_Protect_AlertSet(FLAG_ALERT_UV);
		LOG_W("Discharge:UV Protect Trigger");
	}
}


void App_Protect_OnDevice(void)
{
	LOG_W("App_Protect_OnDevice");
}

void App_Protect_OnOvrd(void)
{
	LOG_W("App_Protect_OnOvrd");
}
