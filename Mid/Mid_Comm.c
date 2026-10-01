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
 * 模块职责:通信协议层实现(CAN 自定义协议)
 *
 * 协议约定(标准帧):
 *   0x200  状态上报帧  BMS -> 主控  周期 2s,负载 8 字节
 *           [0]系统模式 [1]SOC(%) [2]最高单体电压(0.1V) [3]最低单体电压(0.1V)
 *           [4]最高温度(℃,有符号) [5]电流(0.1A,有符号) [6..7]报警标志(低字节在前)
 *   0x100  控制命令帧  主控 -> BMS  负载 2 字节
 *           [0]控制对象(1=充电,2=放电,3=均衡) [1]动作(0=关,1=开)
 *   0x180  命令应答帧  BMS -> 主控  负载 2 字节(回显对象+动作)
 */
#include <string.h>
#include <stdint.h>
#include <rtthread.h>

#include "Mid_Comm.h"

#include "Dri_CAN.h"
#include "can.h"

#include "App_Global.h"
#include "App_Monitor.h"
#include "App_Analysis.h"
#include "App_Protect.h"

#include "Com_Type.h"

#define DBG_TAG "comm"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"


// p_thread config
#define COMM_TASK_STACK_SIZE	512
#define COMM_TASK_PRIORITY		13
#define COMM_TASK_TIMESLICE		25

// 线程轮询周期:命令处理需要较快响应
#define COMM_TASK_PERIOD		100
// 状态上报周期
#define COMM_REPORT_PERIOD_MS	2000


static void Mid_Comm_TaskEntry(void *parameter);
static void Mid_Comm_SendStdFrame(uint32_t std_id, const uint8_t *p_data, uint8_t length);
static void Mid_Comm_SendStatusFrame(void);
static void Mid_Comm_SendAckFrame(uint8_t obj, uint8_t act);
static void Mid_Comm_ProcessRx(void);


void Mid_Comm_Init(void)
{
	rt_thread_t p_thread;

	p_thread = rt_thread_create("comm",
							   Mid_Comm_TaskEntry,
							   NULL,
							   COMM_TASK_STACK_SIZE,
							   COMM_TASK_PRIORITY,
							   COMM_TASK_TIMESLICE);

	if (p_thread == NULL)
	{
		LOG_E("Create Task Fail");
	}

	rt_thread_startup(p_thread);
}


// 投递一帧标准数据帧到发送队列(由 can_send 线程发出)
static void Mid_Comm_SendStdFrame(uint32_t std_id, const uint8_t *p_data, uint8_t length)
{
	CanTxMessage_Struct st_msg;

	memset(&st_msg, 0, sizeof(st_msg));
	st_msg.header.StdId = std_id;
	st_msg.header.IDE = CAN_ID_STD;
	st_msg.header.RTR = CAN_RTR_DATA;
	st_msg.header.DLC = length;
	st_msg.header.TransmitGlobalTime = DISABLE;
	memcpy(st_msg.buffer, p_data, length);

	Dri_CAN_SendMessageToQueue(&st_msg);
}


// 状态周期上报帧(0x200)
static void Mid_Comm_SendStatusFrame(void)
{
	uint8_t data[8] = {0};
	int16_t curr_01a;
	int16_t temp_max = 0;
	BmsProtectAlertType alert = App_Protect_GetAlert();

	// [0] 系统模式(CHARGE=1/DISCHARGE=2/STANDBY=3/SLEEP=4)
	data[0] = (uint8_t)g_st_global_param.sys_mode;

	// [1] SOC(0~100 %)
	data[1] = (uint8_t)(g_st_analysis_data.soc * 100.0f);

	// [2][3] 最高/最低单体电压(0.1V)
	data[2] = (uint8_t)(g_st_analysis_data.cell_volt_max * 10.0f);
	data[3] = (uint8_t)(g_st_analysis_data.cell_volt_min * 10.0f);

	// [4] 最高温度(℃,有符号)
	if (g_st_monitor_data.cell_temp_effective_number > 0)
	{
		temp_max = (int16_t)g_st_monitor_data.cell_temp[g_st_monitor_data.cell_temp_effective_number - 1];
	}
	data[4] = (uint8_t)(int8_t)temp_max;

	// [5] 电流(0.1A,有符号,范围 -12.8~12.7A)
	curr_01a = (int16_t)(g_st_monitor_data.battery_current * 10.0f);
	if (curr_01a > 127)  curr_01a = 127;
	if (curr_01a < -128) curr_01a = -128;
	data[5] = (uint8_t)(int8_t)curr_01a;

	// [6][7] 报警标志(低字节在前)
	data[6] = (uint8_t)(alert & 0xFFu);
	data[7] = (uint8_t)((alert >> 8) & 0xFFu);

	Mid_Comm_SendStdFrame(MID_COMM_FRAME_STATUS, data, sizeof(data));
}


// 命令应答帧(0x180):回显对象 + 动作
static void Mid_Comm_SendAckFrame(uint8_t obj, uint8_t act)
{
	uint8_t data[2];

	data[0] = obj;
	data[1] = act;

	Mid_Comm_SendStdFrame(MID_COMM_FRAME_ACK, data, sizeof(data));
}


// 处理主控下发的控制命令(0x100)
static void Mid_Comm_ProcessRx(void)
{
	CanRxMessage_Struct st_msg;

	// 非阻塞取出并清空接收队列
	while (Dri_CAN_RecvMessageFromQueue(&st_msg, 0) == RT_EOK)
	{
		// 只处理标准帧的控制命令
		if (st_msg.rx_header.IDE != CAN_ID_STD ||
			st_msg.rx_header.StdId != MID_COMM_FRAME_CMD ||
			st_msg.rx_header.DLC < 2)
		{
			continue;
		}

		uint8_t obj = st_msg.buffer[0];
		uint8_t act = st_msg.buffer[1];
		BmsStateType state = (act == MID_COMM_ACT_ENABLE) ? BMS_STATE_ENABLE : BMS_STATE_DISABLE;

		switch (obj)
		{
			case MID_COMM_OBJ_CHARGE:
				g_st_global_param.charge_allowed = state;
				break;
			case MID_COMM_OBJ_DISCHARGE:
				g_st_global_param.discharge_allowed = state;
				break;
			case MID_COMM_OBJ_BALANCE:
				g_st_global_param.balance_allowed = state;
				break;
			default:
				continue;	// 未知对象,不应答
		}

		LOG_I("CAN cmd: obj=%d act=%d", obj, act);
		Mid_Comm_SendAckFrame(obj, act);
	}
}


// 通信线程:周期上报状态 + 处理控制命令
static void Mid_Comm_TaskEntry(void *parameter)
{
	uint32_t report_elapsed_ms = 0;

	while(1)
	{
		// 每 2s 上报一次状态
		report_elapsed_ms += COMM_TASK_PERIOD;
		if (report_elapsed_ms >= COMM_REPORT_PERIOD_MS)
		{
			report_elapsed_ms = 0;
			Mid_Comm_SendStatusFrame();
		}

		// 处理主控下发的控制命令
		Mid_Comm_ProcessRx();

		rt_thread_mdelay(COMM_TASK_PERIOD);
	}
}