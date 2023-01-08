#include <stdio.h>
#include <string.h>

#include <rtthread.h>

#include "test_app.h"

#include "bms_monitor.h"
#include "bms_global.h"

#include "drv_can.h"





#define DBG_TAG "test"
#define DBG_LVL DBG_LOG
#include "rtdbg.h"




#define CAN_FRAME_ID_BMS_CMD	0x101
#define CAN_FRAME_ID_BMS_STATE	0x102


#define OFFSET_READ_BIT(DATA, POS, MASK) (((DATA) >> (POS)) & (MASK))



#define BMS_CMD_Pos				(1U)
#define BMS_CMD_Msk				(0x1U)

#define BMS_CMD_DSG_Pos			(2U)
#define BMS_CMD_DSG_Msk			(0x3U)
	
#define BMS_CMD_CHG_Pos			(4U)
#define BMS_CMD_CHG_Msk			(0x3U)

#define BMS_CMD_BALAN_Pos		(6U)
#define BMS_CMD_BALAN_Msk		(0x3U)




BMS_CommDataRecvTypedef BMS_CommDataRecv = {0};




static void Test_StepTaskEntry(void *paramter);
static void Test_ParseTaskEntry(void *paramter);





void Test_Initialize(void)
{
	rt_thread_t thread;

	thread = rt_thread_create("step",
							   Test_StepTaskEntry,
							   NULL,
							   1024,
							   21,
							   20);

	if (thread == NULL)
	{
		LOG_E("Create Task 1 Fail");
	}

	rt_thread_startup(thread);



	thread = rt_thread_create("parse",
							   Test_ParseTaskEntry,
							   NULL,
							   512,
							   24,
							   20);

	if (thread == NULL)
	{
		LOG_E("Create Task 2 Fail");
	}

	rt_thread_startup(thread);	
}





static void Test_StepTaskEntry(void *paramter)
{
	while(1)
	{
		Runnable_Step();
		rt_thread_mdelay(10);
	}
}


static void Test_ParseTaskEntry(void *paramter)
{
	CAN_RxMessageTypedef RxMessage;
	
	while(1)
	{
		if (CAN_RecvMsgFromMq(&RxMessage, RT_WAITING_FOREVER) != RT_EOK)
		{
			continue;
		}

		if (RxMessage.RxHeader.IDE != CAN_ID_STD)
		{
			LOG_W("can rx frame format not std");
			continue;
		}

		if (RxMessage.RxHeader.RTR != CAN_RTR_DATA)
		{
			LOG_W("can rx frame type not data");
			continue;
		}

		if (RxMessage.RxHeader.StdId != CAN_FRAME_ID_BMS_CMD)
		{
			LOG_W("can rx frame id not %04x", CAN_FRAME_ID_BMS_CMD);
			continue;
		}
		

		BMS_CommDataRecv.BmsCmd = OFFSET_READ_BIT(RxMessage.buffer[0], BMS_CMD_Pos, BMS_CMD_Msk);
		BMS_CommDataRecv.Balance = OFFSET_READ_BIT(RxMessage.buffer[0], BMS_CMD_BALAN_Pos, BMS_CMD_BALAN_Msk);
       	BMS_CommDataRecv.Discharge = OFFSET_READ_BIT(RxMessage.buffer[0], BMS_CMD_DSG_Pos, BMS_CMD_DSG_Msk);
       	BMS_CommDataRecv.Charge = OFFSET_READ_BIT(RxMessage.buffer[0], BMS_CMD_CHG_Pos, BMS_CMD_CHG_Msk);

       	/*
		rt_kprintf("data: ");

        for (uint8_t i = 0; i < RxMessage.RxHeader.DLC; i++)
        {
            rt_kprintf("%d", RxMessage.buffer[i]);
       	}
       	*/
	}
}



void Bms_CommDataSend(BMS_CommDataSendTypedef type, Double dat)
{
	static uint8_t mask = 0;
	static CAN_TxMessageTypedef msg = {{CAN_FRAME_ID_BMS_STATE, 0, CAN_ID_STD, CAN_RTR_DATA, 8, DISABLE,},{0}};
	
	
	//LOG_W("%d %d", rt_tick_get(), type);
	
	switch(type)
	{
		case ST_BUS_VOLTAGE:
		{
			msg.buffer[1] |= ((uint32_t)(dat * 1000)) & 0xFF;
			msg.buffer[0] |= (((uint32_t)(dat * 1000)) >> 8) & 0xFF;

			mask |= ST_BUS_VOLTAGE;
		}break;


		case ST_BATT_FAULT:
		{
			msg.buffer[3] |= ((uint32_t)dat) & 0x03;

			mask |= ST_BATT_FAULT;
		}break;
		

		case ST_CURRENT:
		{
			msg.buffer[2] |= (((uint32_t)((dat + 8.192) * 1000)) >> 6) & 0xFF;
			msg.buffer[3] |= (((uint32_t)((dat + 8.192) * 1000)) << 2) & 0xFC;

			mask |= ST_CURRENT;
		}break;


		case ST_BATT_STATE:
		{
			msg.buffer[5] |= ((uint32_t)dat) & 0x03;

			mask |= ST_BATT_STATE;
		}break;


		case ST_SOC:
		{
			msg.buffer[4] |= (((uint32_t)(dat  * 100)) >> 6) & 0xFF;
			msg.buffer[5] |= (((uint32_t)(dat  * 100)) << 2) & 0xFC;

			mask |= ST_SOC;
		}break;
	}



	if (mask == 0x1F)
	{
		mask = 0;
		CAN_SendMsgToMq(&msg);
		memset(msg.buffer, 0, sizeof(msg.buffer));
	}
}


