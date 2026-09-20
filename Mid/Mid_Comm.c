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
 * 模块职责:通信协议层占位实现
 */
#include <stddef.h>
#include <rtthread.h>

#include "Mid_Comm.h"


#define DBG_TAG "comm"
#define DBG_LVL DBG_LOG
#include "Com_Log.h"




// p_thread config
#define COMM_TASK_STACK_SIZE	512	
#define COMM_TASK_PRIORITY		13
#define COMM_TASK_TIMESLICE		25

#define COMM_TASK_PERIOD		2000






static void Mid_Comm_TaskEntry(void *parameter);


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


/* 通信协议尚未实现:线程当前只做空转,协议落地后在此处理收发 */
static void Mid_Comm_TaskEntry(void *parameter)
{
	while(1)
	{	
		rt_thread_mdelay(COMM_TASK_PERIOD);
	}
}



