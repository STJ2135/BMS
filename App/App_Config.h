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
 * 模块职责:业务配置(保护阈值、SOC 与均衡参数、电池容量)
 */
#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "Int_BQ769X0.h"

// 默认电池额定容量值(Ah)
// 这个值没有实际用容量测试仪校准过,是卖家口头说的
#define BMS_BATTERY_CAPACITY	2.2

// 在待机模式下静止多少时间进入睡眠模式(Min),睡眠低功耗处理暂未考虑
#define BMS_ENTRY_SLEEP_TIME	60

/***************************** 电池保护相关参数 ***********************************/
// 三元锂电池(Ternary lithium battery)默认参数
#define TLB_OV_PROTECT			4.20	// 单体过压保护电压
#define TLB_OV_RELEASE			4.00	// 单体过压恢复电压
#define TLB_UV_PROTECT			3.10	// 单体欠压保护电压
#define TLB_UV_RELEASE			3.15	// 单体欠压恢复电压
#define TLB_SHUTDOWN_VOLTAGE	3.08	// 自动关机电压
#define TLB_BALANCE_VOLTAGE		3.30	// 均衡起始电压

// 磷酸铁锂电池(lithium iron phosphate battery)默认参数
#define LIPB_OV_PROTECT			3.60	// 单体过压保护电压
#define LIPB_OV_RELEASE			3.55	// 单体过压恢复电压
#define LIPB_UV_PROTECT			2.60	// 单体欠压保护电压
#define LIPB_UV_RELEASE			2.65	// 单体欠压恢复电压
#define LIPB_SHUTDOWN_VOLTAGE	2.50	// 自动关机电压
#define LIPB_BALANCE_VOLTAGE	3.00	// 均衡起始电压

// 钛酸锂电池(Lithium titanate battery)默认参数
#define LTB_OV_PROTECT			2.70	// 单体过压保护电压
#define LTB_OV_RELEASE			2.65	// 单体过压恢复电压
#define LTB_UV_PROTECT			1.80	// 单体欠压保护电压
#define LTB_UV_RELEASE			1.85	// 单体欠压恢复电压
#define LTB_SHUTDOWN_VOLTAGE	1.70	// 自动关机电压
#define LTB_BALANCE_VOLTAGE		2.30	// 均衡起始电压

// 初始默认值(当前选用三元锂参数)
#define INIT_OV_PROTECT			TLB_OV_PROTECT			// 单体过压保护电压(V)(注意BQ769X0 OV范围:3.15~4.70V)
#define INIT_OV_RELEASE			TLB_OV_RELEASE			// 单体过压恢复电压(V)
#define INIT_UV_PROTECT			TLB_UV_PROTECT			// 单体欠压保护电压(V)(注意BQ769X0 UV范围:1.58~3.10V)
#define INIT_UV_RELEASE			TLB_UV_RELEASE			// 单体欠压恢复电压(V)

#define INIT_SHUTDOWN_VOLTAGE	TLB_SHUTDOWN_VOLTAGE	// 自动关机电压(V),暂未使用,预留
#define INIT_BALANCE_VOLTAGE	TLB_BALANCE_VOLTAGE		// 均衡起始电压(V)

#define INIT_BALANCE_CURRENT_MAX	0.6		// 最大均衡电流(A),暂未使用,预留
#define INIT_OCC_MAX			2.2				// 最大充电电流(A)
#define INIT_OCD_MAX			2.2				// 最大放电电流(A),由BQ芯片控制,此参数改动不起作用,应在Int_BQ769X0.c修改放电过流阈值

#define INIT_OV_DELAY			BMS_OV_DELAY_2S		// 充电过压保护延时时间
#define INIT_UV_DELAY			BMS_UV_DELAY_4S		// 放电欠压保护延时时间

#define INIT_OCD_DELAY			BMS_OCD_DELAY_320MS	// 放电过流延时时间
#define INIT_OCD_RELEASE		60					// 放电过流解除时间(S)

#define INIT_SCD_DELAY			BMS_SCD_DELAY_100US	// 放电短路延时时间
#define INIT_SCD_RELEASE		60					// 放电短路解除时间(S)

#define INIT_OCC_DELAY			1					// 充电过流延时时间(S)
#define INIT_OCC_RELEASE		60					// 充电过流解除时间(S)

#define INIT_OTC_PROTECT		70					// 充电过温保护(℃)
#define INIT_OTC_RELEASE		60					// 充电过温解除(℃)

#define INIT_OTD_PROTECT		70					// 放电过温保护(℃)
#define INIT_OTD_RELEASE		60					// 放电过温解除(℃)

#define INIT_LTC_PROTECT		-20					// 充电低温保护(℃)
#define INIT_LTC_RELEASE		-10					// 充电低温解除(℃)

#define INIT_LTD_PROTECT		-20					// 放电低温保护(℃)
#define INIT_LTD_RELEASE		-10					// 放电低温解除(℃)
/*************************************************************************************/

/***************************** 能量管理相关参数 ***********************************/
#define SOC_STOP_CHG_VALUE		1		// 停止充电SOC值
#define SOC_START_CHG_VALUE		0.90	// 启动充电SOC值
#define SOC_STOP_DSG_VALUE		0		// 停止放电SOC值
#define SOC_START_DSG_VALUE		0.10	// 启动放电SOC值

#define BALANCE_DIFF_VOLTAGE	0.05	// 均衡差异电压(V)
#define BALANCE_CYCLE_TIME		30		// 均衡周期时间(s)
#define BALANCE_VOLT_RISE_DELAY	5000	// 均衡电压回升延时(ms)
/*************************************************************************************/

#endif
