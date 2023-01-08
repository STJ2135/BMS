#ifndef __TEST_H__
#define __TEST_H__


#include "Rte_test.h"



// 通信数据接收类型
typedef struct
{
	Double BmsCmd;
	Double Balance;
	Double Charge;
	Double Discharge;
}BMS_CommDataRecvTypedef;


// 通信数据发送类型
typedef enum
{
	ST_BUS_VOLTAGE	= 0x01,
	ST_BATT_FAULT	= 0x02,
	ST_CURRENT		= 0x04,
	ST_BATT_STATE	= 0x08,
	ST_SOC			= 0x10,
}BMS_CommDataSendTypedef;



extern BMS_CommDataRecvTypedef BMS_CommDataRecv;


void Test_Initialize(void);

void Bms_CommDataSend(BMS_CommDataSendTypedef type, Double dat);


#endif


