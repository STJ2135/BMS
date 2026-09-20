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
 * 模块职责:调试命令(msh):充放电/均衡使能、负载检测、信息打印
 */
#include <rtthread.h>
#include <stdio.h>

#include "Int_BQ769X0.h"

#include "App_Monitor.h"
#include "App_Energy.h"
#include "App_Info.h"
#include "App_Global.h"

#define DBG_TAG "cmd"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"

/***************************************** 控制类 *********************************/

// 睡眠唤醒由系统控制,不对外提供命令

static void App_Shell_OpenDischarge(void)
{
	g_st_global_param.discharge_allowed = BMS_STATE_ENABLE;
}
MSH_CMD_EXPORT_ALIAS(App_Shell_OpenDischarge, open_dsg, Open DSG);

static void App_Shell_CloseDischarge(void)
{
	g_st_global_param.discharge_allowed = BMS_STATE_DISABLE;
}
MSH_CMD_EXPORT_ALIAS(App_Shell_CloseDischarge, close_dsg, Close DSG);

static void App_Shell_OpenCharge(void)
{
	g_st_global_param.charge_allowed = BMS_STATE_ENABLE;
}
MSH_CMD_EXPORT_ALIAS(App_Shell_OpenCharge, open_chg, Open CHG);

static void App_Shell_CloseCharge(void)
{
	g_st_global_param.charge_allowed = BMS_STATE_DISABLE;
}
MSH_CMD_EXPORT_ALIAS(App_Shell_CloseCharge, close_chg, Close CHG);

static void App_Shell_OpenBalance(void)
{
	g_st_global_param.balance_allowed = BMS_STATE_ENABLE;
}
MSH_CMD_EXPORT_ALIAS(App_Shell_OpenBalance, open_balance, Open Balance);

static void App_Shell_CloseBalance(void)
{
	g_st_global_param.balance_allowed = BMS_STATE_DISABLE;
}
MSH_CMD_EXPORT_ALIAS(App_Shell_CloseBalance, close_balance, Close Balance);

static void App_Shell_LoadDetect(void)
{
	if (Int_BQ769X0_IsLoadDetected() == true)
	{
		LOG_I("Load Detected");
	}
	else
	{
		LOG_I("No Load Was Detected");
	}
}
MSH_CMD_EXPORT_ALIAS(App_Shell_LoadDetect, load_detect, Load Detect);

/***************************************** 信息类 *********************************/

static void App_Shell_OpenInfo(void)
{
	App_Info_SetPrintEnable(BMS_STATE_ENABLE);
}
MSH_CMD_EXPORT_ALIAS(App_Shell_OpenInfo, open_info, Open Info Printf);

static void App_Shell_CloseInfo(void)
{
	App_Info_SetPrintEnable(BMS_STATE_DISABLE);
}
MSH_CMD_EXPORT_ALIAS(App_Shell_CloseInfo, close_info, Close Info Printf);

static void App_Shell_ShowInfo(void)
{
	App_Info_Print();
}
MSH_CMD_EXPORT_ALIAS(App_Shell_ShowInfo, show_info, Show Info);

/**********************************************************************************/
