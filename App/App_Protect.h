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
 * 模块职责:电池保护:保护参数/报警标志定义与对外接口
 */
#ifndef __APP_PROTECT_H__
#define __APP_PROTECT_H__



#include "App_Config.h"




// 跟保护相关的参数结构体
typedef struct
{
	float shutdown_voltage;	// 关机电压(V)
	
	float ov_protect;		// 充电过压保护电压(V)					由硬件完成
	float ov_release;		// 充电过压恢复电压(V)
	float uv_protect;		// 放电欠压保护电压(V)					由硬件完成
	float uv_release;		// 放电欠压恢复电压(V)
	
	float occ_protect;				// 充电过流阈值(A)
	float ocd_protect;				// 放电过流阈值(A)			由硬件完成
	
	BmsOvDelayType ov_delay;		// 充电过压保护延时				由硬件完成
	BmsUvDelayType uv_delay;		// 放电欠压保护延时				由硬件完成
	BmsOcdDelayType ocd_delay;	// 放电过流保护延时				由硬件完成
	BmsScdDelayType scd_delay;	// 放电短路保护延时				由硬件完成
	
	uint8_t ocd_release;		// 放电过流恢复(S)
	uint8_t scd_release;		// 放电短路恢复(S)
	
	uint8_t occ_delay;		// 充电过流延时(S)
	uint8_t occ_release;		// 充电过流恢复(S)
	
	float otc_protect;		// 充电过温保护(℃)
	float otc_release;		// 充电过温解除(℃)
	float otd_protect;		// 放电过温保护(℃)
	float otd_release;		// 放电过温解除(℃)
	
	float ltc_protect;		// 充电低温保护(℃)
	float ltc_release;		// 充电低温解除(℃)
	float ltd_protect;		// 放电低温保护(℃)
	float ltd_release;		// 放电低温解除(℃)
}BmsProtectParam_Struct;




// 报警枚举体
typedef enum
{
	FLAG_ALERT_NO	= 0x0000,		// 无报警触发
	
	FLAG_ALERT_OV	= 0X0001,		// 充电过压保护触发位				硬件触发
	FLAG_ALERT_OCC	= 0X0002,		// 充电过流保护触发位				软件触发
	FLAG_ALERT_OTC	= 0X0004,		// 充电过温保护触发位				软件触发
	FLAG_ALERT_LTC	= 0X0008,		// 充电低温保护触发位				软件触发

	FLAG_ALERT_CHG_MASK = 0x000F,	// 充电报警掩码
	
	FLAG_ALERT_UV	= 0X0010,		// 放电欠压保护触发位				硬件触发
	FLAG_ALERT_OCD	= 0X0020,		// 放电过流保护触发位				硬件触发
	FLAG_ALERT_SCD	= 0X0040,		// 放电短路保护触发位				硬件触发
	FLAG_ALERT_OTD	= 0X0080,		// 放电过温保护触发位				软件触发
	FLAG_ALERT_LTD	= 0X0100,		// 放电低温保护触发位				软件触发

	FLAG_ALERT_DSG_MASK = 0x01F0,	// 放电报警掩码
}BmsProtectAlertType;




// 保护参数 + 当前报警标志(alert 为位掩码,见 BmsProtectAlertType)
typedef struct
{
	BmsProtectParam_Struct param;
	BmsProtectAlertType alert;
}BmsProtect_Struct;


extern BmsProtect_Struct g_st_protect;



// 创建并启动保护线程(周期 200ms)
void App_Protect_Init(void);
// 读取当前报警标志(内部进临界区,可与中断安全并发)
BmsProtectAlertType App_Protect_GetAlert(void);

// 以下为 AFE 硬件报警回调,在 ALERT 中断中执行:立即锁断对应 MOS 并置报警位
void App_Protect_OnOcd(void);
void App_Protect_OnScd(void);
void App_Protect_OnOv(void);
void App_Protect_OnUv(void);


// 器件故障 / ALERT 被外部电路强行拉高 的回调
void App_Protect_OnDevice(void);
void App_Protect_OnOvrd(void);



#endif


