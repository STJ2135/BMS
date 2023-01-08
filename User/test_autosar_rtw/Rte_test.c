#include "Rte_test.h"

#include "bms_monitor.h"
#include "bms_global.h"

#include "test_app.h"



// 读取电池电流
Double Rte_IRead_test_Runnable_Step_current_current(void)
{
	return (Double)BMS_MonitorData.BatteryCurrent;
}

// 读取第一串电池电压
Double Rte_IRead_test_Runnable_Step_voltage_V1_voltage_V1(void)
{
	return (Double)BMS_MonitorData.CellVoltage[0];
}

// 读取第二串电池电压
Double Rte_IRead_test_Runnable_Step_voltage_V2_voltage_V2(void)
{
	return (Double)BMS_MonitorData.CellVoltage[1];
}

// 读取第三串电池电压
Double Rte_IRead_test_Runnable_Step_voltage_V3_voltage_V3(void)
{
	return (Double)BMS_MonitorData.CellVoltage[2];
}

// 读取第四串电池电压
Double Rte_IRead_test_Runnable_Step_voltage_V4_voltage_V4(void)
{
	return (Double)BMS_MonitorData.CellVoltage[3];
}

// 读取第五串电池电压
Double Rte_IRead_test_Runnable_Step_voltage_V5_voltage_V5(void)
{
	return (Double)BMS_MonitorData.CellVoltage[4];
}












// 读取bms cmd段
Double Rte_IRead_test_Runnable_Step_BMS_Cmd_BMS_Cmd(void)
{
	return BMS_CommDataRecv.BmsCmd;
}

// 读取均衡数据段
Double Rte_IRead_test_Runnable_Step_BMS_CmdOpenBalan_BMS_CmdOpenBalan(void)
{
	return BMS_CommDataRecv.Balance;
}

// 读取充电开关段
Double Rte_IRead_test_Runnable_Step_BMS_CmdOpenCHG_BMS_CmdOpenCHG(void)
{
	return BMS_CommDataRecv.Charge;
}

// 读取放电开关段
Double Rte_IRead_test_Runnable_Step_BMS_CmdOpenDSG_BMS_CmdOpenDSG(void)
{
	return BMS_CommDataRecv.Discharge;
}


















// 开启均衡
void Rte_IWrite_test_Runnable_Step_OutCmdOpenBalan_OutCmdOpenBalan(Double u)
{
	u == 1 ? (BMS_GlobalParam.Balance = BMS_STATE_ENABLE) : 0;
}
Double* Rte_IWriteRef_test_Runnable_Step_OutCmdOpenBalan_OutCmdOpenBalan(void)
{

}


// 关闭均衡
void Rte_IWrite_test_Runnable_Step_OutCmdCloseBalan_OutCmdCloseBalan(Double u)
{
	u == 1 ? (BMS_GlobalParam.Balance = BMS_STATE_DISABLE) : 0;
}
Double* Rte_IWriteRef_test_Runnable_Step_OutCmdCloseBalan_OutCmdCloseBalan(void)
{

}


// 开启充电
void Rte_IWrite_test_Runnable_Step_OutCmdOpenCHG_OutCmdOpenCHG(Double u)
{
	u == 1 ? (BMS_GlobalParam.Charge = BMS_STATE_ENABLE) : 0;
}
Double* Rte_IWriteRef_test_Runnable_Step_OutCmdOpenCHG_OutCmdOpenCHG(void)
{

}


// 关闭充电
void Rte_IWrite_test_Runnable_Step_OutCmdCloseCHG_OutCmdCloseCHG(Double u)
{
	u == 1 ? (BMS_GlobalParam.Charge = BMS_STATE_DISABLE) : 0;
}
Double* Rte_IWriteRef_test_Runnable_Step_OutCmdCloseCHG_OutCmdCloseCHG(void)
{

}


// 开启放电
void Rte_IWrite_test_Runnable_Step_OutCmdOpenDSG_OutCmdOpenDSG(Double u)
{
	u == 1 ? (BMS_GlobalParam.Discharge = BMS_STATE_ENABLE) : 0;
}
Double* Rte_IWriteRef_test_Runnable_Step_OutCmdOpenDSG_OutCmdOpenDSG(void)
{

}


// 关闭放电
void Rte_IWrite_test_Runnable_Step_OutCmdCloseDSG_OutCmdCloseDSG(Double u)
{
	u == 1 ? (BMS_GlobalParam.Discharge = BMS_STATE_DISABLE) : 0;
}
Double* Rte_IWriteRef_test_Runnable_Step_OutCmdCloseDSG_OutCmdCloseDSG(void)
{

}





















// 输出电池包电压
void Rte_IWrite_test_Runnable_Step_OutBus_Voltage_OutBus_Voltage(Double u)
{
	Bms_CommDataSend(ST_BUS_VOLTAGE, u);
}
Double* Rte_IWriteRef_test_Runnable_Step_OutBus_Voltage_OutBus_Voltage(void)
{

}


// 电池故障
void Rte_IWrite_test_Runnable_Step_Batt_Fault_Batt_Fault(Double u)
{
	Bms_CommDataSend(ST_BATT_FAULT, u);
}
Double* Rte_IWriteRef_test_Runnable_Step_Batt_Fault_Batt_Fault(void)
{

}


// 输出电流
void Rte_IWrite_test_Runnable_Step_OutCurrent_OutCurrent(Double u)
{
	Bms_CommDataSend(ST_CURRENT, u);
}
Double* Rte_IWriteRef_test_Runnable_Step_OutCurrent_OutCurrent(void)
{

}


// 读取BMS状态：充电、待机、放电
void Rte_IWrite_test_Runnable_Step_Batt_State_Batt_State(Double u)
{
	Bms_CommDataSend(ST_BATT_STATE, u);
}
Double* Rte_IWriteRef_test_Runnable_Step_Batt_State_Batt_State(void)
{

}


// 输出SOC值
void Rte_IWrite_test_Runnable_Step_SOC_SOC(Double u)
{
	Bms_CommDataSend(ST_SOC, u);
}
Double* Rte_IWriteRef_test_Runnable_Step_SOC_SOC(void)
{

}






