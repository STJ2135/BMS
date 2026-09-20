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
 * 模块职责:全局参量定义与初值
 */
#include "App_Global.h"

BmsGlobalParam_Struct g_st_global_param = 
{
	.sys_mode 			= BMS_MODE_STANDBY,
	.cell_real_number 	= BMS_CELL_MAX,
	.temp_real_number 	= BMS_TEMP_MAX,
	.charge_allowed 			= BMS_STATE_DISABLE,
	.discharge_allowed 			= BMS_STATE_DISABLE,
	.balance_allowed		 	= BMS_STATE_DISABLE,
};



