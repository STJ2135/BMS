/*
 * 应用层入口
 *
 * 分层与依赖方向(上层依赖下层):
 *   App  业务逻辑:监控/保护/分析/能量/信息/命令,以及充放电输出所有者 App_Power
 *   Mid  与硬件无关的软件服务(通信协议,当前为占位)
 *   Int  外部硬件接口层:AFE(BQ769X0)、RS485
 *   Dri  片上外设驱动:软件 I2C、CAN、GPIO 指示灯
 *   Com  通用工具与基础类型/配置
 *
 * 线程模型:各业务模块在 App_BMS_StartModules() 中创建自己的线程
 *   监控(250ms) -> 保护(200ms) -> 分析(1s) -> 能量(200ms) -> 信息(2s)
 * 数据流:AFE 采样 -> 监控汇总 -> 保护/分析/能量消费 -> 信息输出
 *
 * 充放电开关只由 App_Power 下发,其它模块通过请求/锁定接口参与决策
 */
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
#include "App_BMS.h"

#include <string.h>

#include "Int_BQ769X0.h"

#include "App_Monitor.h"
#include "App_Protect.h"
#include "App_Analysis.h"
#include "App_Energy.h"
#include "App_Info.h"
#include "Mid_Comm.h"
#include "App_Global.h"
#include "App_Config.h"
#include "App_Power.h"

// 电压单位换算:V -> mV
#define APP_BMS_VOLT_TO_MV(volt)	((uint16_t)((volt) * 1000))

static void App_BMS_BuildAfeConfig(Bq769x0InitData_Struct *p_config);
static void App_BMS_StartModules(void);

// 应用入口:组装 AFE 配置 -> 初始化 AFE -> 启动各业务模块
void App_BMS_Init(void)
{
	Bq769x0InitData_Struct st_afe_config;

	App_BMS_BuildAfeConfig(&st_afe_config);
	Int_BQ769X0_Init(&st_afe_config);

	App_Power_Init();

	App_BMS_StartModules();
}

// 组装 AFE(BQ769X0)初始化参数:硬件报警回调 + 保护参数
static void App_BMS_BuildAfeConfig(Bq769x0InitData_Struct *p_config)
{
	memset(p_config, 0, sizeof(Bq769x0InitData_Struct));

	// 硬件报警回调:过流/短路/过压/欠压/器件故障/外部强触发由保护模块处理
	p_config->alert_ops.ocd		= App_Protect_OnOcd;
	p_config->alert_ops.scd		= App_Protect_OnScd;
	p_config->alert_ops.ov		= App_Protect_OnOv;
	p_config->alert_ops.uv		= App_Protect_OnUv;
	p_config->alert_ops.device	= App_Protect_OnDevice;
	p_config->alert_ops.ovrd	= App_Protect_OnOvrd;

	// 库仑计采样完成回调由监控模块处理
	// 注意:硬件中断通知方式下,烧写程序后必须重新上下电或复位 AFE 才会生效
	p_config->alert_ops.cc		= App_Monitor_OnCcSampleReady;

	// 保护参数(延时档位宏本身即为对应枚举量,无需强制类型转换)
	p_config->config_data.scd_delay		= INIT_SCD_DELAY;
	p_config->config_data.ocd_delay		= INIT_OCD_DELAY;
	p_config->config_data.uv_delay		= INIT_UV_DELAY;
	p_config->config_data.ov_delay		= INIT_OV_DELAY;
	p_config->config_data.uvp_threshold = APP_BMS_VOLT_TO_MV(INIT_UV_PROTECT);
	p_config->config_data.ovp_threshold = APP_BMS_VOLT_TO_MV(INIT_OV_PROTECT);
}

// 按依赖顺序启动各业务模块
// 监控模块最先启动,保护/分析/能量/信息均依赖它采集的数据
// 各模块只创建并启动线程;本函数运行在主线程(优先级高于所有业务线程),
// 因此可以保证所有业务线程真正获得调度前,本函数已经执行完毕
static void App_BMS_StartModules(void)
{
	App_Monitor_Init();
	App_Protect_Init();
	App_Analysis_Init();
	App_Energy_Init();
	App_Info_Init();
	Mid_Comm_Init();
}
