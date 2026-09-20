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
 * 模块职责:电池状态分析:分析数据结构与对外接口
 */
#ifndef __APP_ANALYSIS_H__
#define __APP_ANALYSIS_H__


#include <stdint.h>




typedef struct
{
	// 以下三项尚未实现,仅保留字段
	uint8_t soh;	// 电池包SOH值			实际容量/额定容量
	uint8_t sop;	// 电池包SOP值	
	uint8_t soe;	// 电池包SOE值


	// 以下两项尚未实现
	uint32_t loop_count;			// 电池包循环次数(完整的一个充放过程+1)
	float capacity_loop;			// 电池包循环容量(Ah)
	

	// 以下为已实现的分析结果
	float soc;					// 电池包SOC值(剩余电量百分比)

	float average_voltage;		// 单体平均电压值(V)
	float max_voltage_difference;	// 单体电芯最大电压差(V)
	float power_real;			// 电池包实时功率(W)
	float cell_volt_max;			// 单体电芯最大电压(V)
	float cell_volt_min;			// 单体电芯最小电压(V)
	
	float capacity_rated;		// 电池包额定容量(Ah)
	float capacity_real;			// 电池包实际容量(Ah)  		计算方法得进行一次完整的充放电计算
	float capacity_remain;		// 电池包剩余容量(Ah)
}BmsAnalysisData_Struct;


extern BmsAnalysisData_Struct g_st_analysis_data;



// 创建并启动分析线程(周期 1000ms)
void App_Analysis_Init(void);



#endif

