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
 * 模块职责:充放电输出管理实现(模块说明见 App_Power.h)
 */
#include "App_Power.h"

#include "Int_BQ769X0.h"

#include "App_Global.h"

// 当前的输出请求(策略层提出,可能被保护锁定否决)
static BmsStateType s_charge_requested = BMS_STATE_DISABLE;
static BmsStateType s_discharge_requested = BMS_STATE_DISABLE;

// 保护锁定标志:锁定时无论请求什么状态,实际输出都强制关断
static bool s_charge_locked = false;
static bool s_discharge_locked = false;

// 命令行许可(g_st_global_param.*_allowed)的快照,用于识别命令行是否改过
static BmsStateType s_charge_allowed_backup = BMS_STATE_DISABLE;
static BmsStateType s_discharge_allowed_backup = BMS_STATE_DISABLE;

static void App_Power_ApplyCharge(void)
{
	BmsStateType output;

	output = (s_charge_locked == true) ? BMS_STATE_DISABLE : s_charge_requested;
	Int_BQ769X0_SetCharge(output);
}

static void App_Power_ApplyDischarge(void)
{
	BmsStateType output;

	output = (s_discharge_locked == true) ? BMS_STATE_DISABLE : s_discharge_requested;
	Int_BQ769X0_SetDischarge(output);
}

// 初始化:输出初值与命令行设置保持一致
void App_Power_Init(void)
{
	s_charge_requested = g_st_global_param.charge_allowed;
	s_discharge_requested = g_st_global_param.discharge_allowed;
	s_charge_allowed_backup = g_st_global_param.charge_allowed;
	s_discharge_allowed_backup = g_st_global_param.discharge_allowed;
	s_charge_locked = false;
	s_discharge_locked = false;

	App_Power_ApplyCharge();
	App_Power_ApplyDischarge();
}

void App_Power_RequestCharge(BmsStateType state)
{
	s_charge_requested = state;
	App_Power_ApplyCharge();
}

void App_Power_RequestDischarge(BmsStateType state)
{
	s_discharge_requested = state;
	App_Power_ApplyDischarge();
}

void App_Power_LockCharge(void)
{
	s_charge_locked = true;
	App_Power_ApplyCharge();
}

void App_Power_LockDischarge(void)
{
	s_discharge_locked = true;
	App_Power_ApplyDischarge();
}

void App_Power_ReleaseCharge(void)
{
	s_charge_locked = false;
	s_charge_requested = g_st_global_param.charge_allowed;
	App_Power_ApplyCharge();
}

void App_Power_ReleaseDischarge(void)
{
	s_discharge_locked = false;
	s_discharge_requested = g_st_global_param.discharge_allowed;
	App_Power_ApplyDischarge();
}

// 仅解除锁定:不主动下发,保持 AFE 当前输出状态
// 用于 OV/UV:AFE 自己已关断,何时重新使能交给能量管理按模式/SOC 判断
void App_Power_UnlockCharge(void)
{
	s_charge_locked = false;
	s_charge_requested = g_st_global_param.charge_allowed;
}

void App_Power_UnlockDischarge(void)
{
	s_discharge_locked = false;
	s_discharge_requested = g_st_global_param.discharge_allowed;
}

// 同步命令行对使能许可的修改:关闭立即生效,开启仅在睡眠模式下立即生效
void App_Power_ApplyCommandChange(void)
{
	if (s_charge_allowed_backup != g_st_global_param.charge_allowed)
	{
		if (g_st_global_param.charge_allowed == BMS_STATE_DISABLE)
		{
			App_Power_RequestCharge(BMS_STATE_DISABLE);
		}
		else if (g_st_global_param.sys_mode == BMS_MODE_SLEEP)
		{
			App_Power_RequestCharge(BMS_STATE_ENABLE);
		}
		s_charge_allowed_backup = g_st_global_param.charge_allowed;
	}

	if (s_discharge_allowed_backup != g_st_global_param.discharge_allowed)
	{
		if (g_st_global_param.discharge_allowed == BMS_STATE_DISABLE)
		{
			App_Power_RequestDischarge(BMS_STATE_DISABLE);
		}
		else if (g_st_global_param.sys_mode == BMS_MODE_SLEEP)
		{
			App_Power_RequestDischarge(BMS_STATE_ENABLE);
		}
		s_discharge_allowed_backup = g_st_global_param.discharge_allowed;
	}
}
