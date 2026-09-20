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
 * 模块职责:能量管理:均衡与充放电策略参数
 */
#ifndef __APP_ENERGY_H__
#define __APP_ENERGY_H__


#include <rtthread.h>

#include "Com_Type.h"


typedef struct
{
	float soc_stop_chg;			// 停止充电SOC值
	float soc_start_chg;			// 启动充电SOC值
	float soc_stop_dsg;			// 停止放电SOC值
	float soc_start_dsg;			// 启动放电SOC值
	
	float balance_start_voltage;	// 均衡起始电压(V)
	float balance_diff_voltage;	// 均衡差异电压(V)
	uint32_t balance_cycle_time;	// 均衡周期时间(s)
	BmsCellMaskType balance_record;	// 正在均衡的电芯位掩码(每 bit 对应一节)
	bool balance_release_flag;			// 表示均衡释放,false:表示已不满足均衡条件,true:满足均衡条件
}BmsEnergyData_Struct;


extern BmsEnergyData_Struct g_st_energy_data;


// 创建并启动能量管理线程(周期 200ms)
void App_Energy_Init(void);



#endif

