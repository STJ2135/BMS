/*
 * File: test.c
 *
 * Code generated for Simulink model 'test'.
 *
 * Model version                  : 1.2
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Tue Jan  3 09:34:37 2023
 *
 * Target selection: autosar.tlc
 * Embedded hardware selection: STMicroelectronics->ST10/Super10
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "test.h"

/* Block signals and states (default storage) */
DW rtDW;
static real_T look1_binlx(real_T u0, const real_T bp0[], const real_T table[],
  uint32_T maxIndex);
static real_T look1_binlx(real_T u0, const real_T bp0[], const real_T table[],
  uint32_T maxIndex)
{
  real_T frac;
  uint32_T iRght;
  uint32_T iLeft;
  uint32_T bpIdx;

  /* Lookup 1-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Linear'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0UL]) {
    iLeft = 0UL;
    frac = (u0 - bp0[0UL]) / (bp0[1UL] - bp0[0UL]);
  } else if (u0 < bp0[maxIndex]) {
    /* Binary Search */
    bpIdx = maxIndex >> 1UL;
    iLeft = 0UL;
    iRght = maxIndex;
    while (iRght - iLeft > 1UL) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1UL;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1UL] - bp0[iLeft]);
  } else {
    iLeft = maxIndex - 1UL;
    frac = (u0 - bp0[maxIndex - 1UL]) / (bp0[maxIndex] - bp0[maxIndex - 1UL]);
  }

  /* Interpolation 1-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'wrapping'
   */
  return (table[iLeft + 1UL] - table[iLeft]) * frac + table[iLeft];
}

/* Model step function */
void Runnable_Step(void)
{
  real_T rtb_Add_i;
  boolean_T rtb_LogicalOperator1_gr;
  boolean_T rtb_Compare_p;
  real_T rtb_Switch;
  real_T rtb_LookupTable;
  int16_T rtb_DataTypeConversion2;

  /* Switch: '<Root>/Switch' incorporates:
   *  Constant: '<Root>/Constant'
   *  Constant: '<Root>/Constant1'
   *  Inport: '<Root>/current'
   *  Switch: '<Root>/Switch1'
   */
  if (Rte_IRead_Runnable_Step_current_current() > 0.2) {
    rtb_Add_i = 1.0;
  } else if (Rte_IRead_Runnable_Step_current_current() > -0.2) {
    /* Switch: '<Root>/Switch1' incorporates:
     *  Constant: '<Root>/Constant2'
     */
    rtb_Add_i = 2.0;
  } else {
    rtb_Add_i = 3.0;
  }

  /* End of Switch: '<Root>/Switch' */

  /* Outport: '<Root>/Batt_State' */
  Rte_IWrite_Runnable_Step_Batt_State_Batt_State(rtb_Add_i);

  /* MinMax: '<S5>/MinMax' incorporates:
   *  Inport: '<Root>/voltage_V1'
   *  Inport: '<Root>/voltage_V2'
   *  Inport: '<Root>/voltage_V3'
   *  Inport: '<Root>/voltage_V4'
   *  Inport: '<Root>/voltage_V5'
   */
  rtb_Add_i = fmin(fmin(fmin(fmin(Rte_IRead_Runnable_Step_voltage_V1_voltage_V1(),
    Rte_IRead_Runnable_Step_voltage_V2_voltage_V2()),
    Rte_IRead_Runnable_Step_voltage_V3_voltage_V3()),
                        Rte_IRead_Runnable_Step_voltage_V4_voltage_V4()),
                   Rte_IRead_Runnable_Step_voltage_V5_voltage_V5());

  /* Delay: '<S4>/Delay' */
  if (rtDW.icLoad != 0) {
    rtDW.Delay_DSTATE = rtb_Add_i;
  }

  /* Sum: '<S4>/Add' incorporates:
   *  Delay: '<S4>/Delay'
   *  Gain: '<S4>/Gain1'
   */
  rtb_Add_i = 0.1 * rtb_Add_i + rtDW.Delay_DSTATE;

  /* Saturate: '<S4>/Saturation' */
  if (rtb_Add_i > 4.25) {
    rtb_Switch = 4.25;
  } else if (rtb_Add_i < 3.4) {
    rtb_Switch = 3.4;
  } else {
    rtb_Switch = rtb_Add_i;
  }

  /* End of Saturate: '<S4>/Saturation' */

  /* Lookup_n-D: '<Root>/Lookup Table' */
  rtb_LookupTable = look1_binlx(rtb_Switch, rtConstP.LookupTable_bp01Data,
    rtConstP.LookupTable_tableData, 8UL);

  /* Delay: '<S1>/Delay' */
  if (rtDW.icLoad_o != 0) {
    rtDW.Delay_DSTATE_m = rtb_LookupTable;
  }

  /* Chart: '<S10>/count' incorporates:
   *  Abs: '<Root>/Abs'
   *  Constant: '<S10>/Constant'
   *  Constant: '<S19>/Constant'
   *  Inport: '<Root>/current'
   *  RelationalOperator: '<S19>/Compare'
   */
  if (fabs(Rte_IRead_Runnable_Step_current_current()) <= 0.1) {
    rtDW.Output1 = fmin(40.0, rtDW.Output1 + 1.0);
  } else {
    rtDW.Output1 = 0.0;
  }

  /* End of Chart: '<S10>/count' */

  /* RelationalOperator: '<S10>/Relational Operator1' incorporates:
   *  Constant: '<S10>/Constant'
   *  Constant: '<S10>/Constant1'
   *  Delay: '<S10>/Delay'
   *  Gain: '<S10>/Gain'
   *  Sum: '<S10>/Add'
   */
  rtb_LogicalOperator1_gr = (20.0 <= (0.1 + rtDW.Delay_DSTATE_b) * 0.1);

  /* Switch: '<S1>/Switch' incorporates:
   *  Constant: '<S10>/Constant'
   *  Delay: '<S1>/Delay'
   *  Gain: '<Root>/Gain'
   *  Gain: '<S1>/Gain1'
   *  Gain: '<S1>/Gain2'
   *  Gain: '<S1>/Gain3'
   *  Inport: '<Root>/current'
   *  Logic: '<S10>/Logical Operator'
   *  RelationalOperator: '<S10>/Relational Operator'
   *  Sum: '<S1>/Add'
   *  Sum: '<S1>/Add1'
   */
  if ((rtDW.Output1 >= 20.0) || rtb_LogicalOperator1_gr) {
    rtb_Switch = (-0.00012626262626262626 *
                  Rte_IRead_Runnable_Step_current_current() * 0.1 +
                  rtDW.Delay_DSTATE_m) * 0.5 + 0.5 * rtb_LookupTable;
  } else {
    rtb_Switch = -0.00012626262626262626 *
      Rte_IRead_Runnable_Step_current_current() * 0.1 + rtDW.Delay_DSTATE_m;
  }

  /* End of Switch: '<S1>/Switch' */

  /* Delay: '<S3>/Delay' */
  if (rtDW.icLoad_j != 0) {
    /* Saturate: '<Root>/Saturation1' */
    if (rtb_LookupTable > 1.0) {
      rtDW.Delay_DSTATE_mm = 1.0;
    } else if (rtb_LookupTable < 0.0) {
      rtDW.Delay_DSTATE_mm = 0.0;
    } else {
      rtDW.Delay_DSTATE_mm = rtb_LookupTable;
    }

    /* End of Saturate: '<Root>/Saturation1' */
  }

  /* Saturate: '<Root>/Saturation' */
  if (rtb_Switch > 1.0) {
    rtb_LookupTable = 1.0;
  } else if (rtb_Switch < 0.0) {
    rtb_LookupTable = 0.0;
  } else {
    rtb_LookupTable = rtb_Switch;
  }

  /* End of Saturate: '<Root>/Saturation' */

  /* Sum: '<S3>/Add' incorporates:
   *  Delay: '<S3>/Delay'
   *  Gain: '<S3>/Gain1'
   */
  rtb_LookupTable = 0.005 * rtb_LookupTable + rtDW.Delay_DSTATE_mm;

  /* Outport: '<Root>/SOC' incorporates:
   *  Gain: '<Root>/Gain1'
   */
  Rte_IWrite_Runnable_Step_SOC_SOC(100.0 * rtb_LookupTable);

  /* Update for Delay: '<S3>/Delay' incorporates:
   *  Gain: '<S3>/Gain2'
   */
  rtDW.Delay_DSTATE_mm = 0.995 * rtb_LookupTable;

  /* Logic: '<S10>/Logical Operator1' */
  rtb_LogicalOperator1_gr = !rtb_LogicalOperator1_gr;

  /* DataTypeConversion: '<S10>/Data Type Conversion2' */
  rtb_DataTypeConversion2 = rtb_LogicalOperator1_gr;

  /* Outport: '<Root>/OutBus_Voltage' incorporates:
   *  Inport: '<Root>/voltage_V1'
   *  Inport: '<Root>/voltage_V2'
   *  Inport: '<Root>/voltage_V3'
   *  Inport: '<Root>/voltage_V4'
   *  Inport: '<Root>/voltage_V5'
   *  Sum: '<S5>/Add'
   */
  Rte_IWrite_Runnable_Step_OutBus_Voltage_OutBus_Voltage
    ((((Rte_IRead_Runnable_Step_voltage_V5_voltage_V5() +
        Rte_IRead_Runnable_Step_voltage_V4_voltage_V4()) +
       Rte_IRead_Runnable_Step_voltage_V3_voltage_V3()) +
      Rte_IRead_Runnable_Step_voltage_V2_voltage_V2()) +
     Rte_IRead_Runnable_Step_voltage_V1_voltage_V1());

  /* RelationalOperator: '<S2>/Compare' incorporates:
   *  Constant: '<S2>/Constant'
   *  Inport: '<Root>/BMS_Cmd'
   */
  rtb_Compare_p = (Rte_IRead_Runnable_Step_BMS_Cmd_BMS_Cmd() == 1.0);

  /* Outport: '<Root>/OutCmdCloseCHG' incorporates:
   *  Constant: '<S14>/Constant'
   *  DataTypeConversion: '<S7>/Data Type Conversion4'
   *  Inport: '<Root>/BMS_CmdOpenCHG'
   *  Logic: '<S7>/Logical Operator'
   *  RelationalOperator: '<S14>/Compare'
   */
  Rte_IWrite_Runnable_Step_OutCmdCloseCHG_OutCmdCloseCHG(rtb_Compare_p &&
    (Rte_IRead_Runnable_Step_BMS_CmdOpenCHG_BMS_CmdOpenCHG() == 1.0));

  /* Outport: '<Root>/OutCmdOpenBalan' incorporates:
   *  Constant: '<S11>/Constant'
   *  DataTypeConversion: '<S6>/Data Type Conversion1'
   *  Inport: '<Root>/BMS_CmdOpenBalan'
   *  Logic: '<S6>/Logical Operator1'
   *  RelationalOperator: '<S11>/Compare'
   */
  Rte_IWrite_Runnable_Step_OutCmdOpenBalan_OutCmdOpenBalan(rtb_Compare_p &&
    (Rte_IRead_Runnable_Step_BMS_CmdOpenBalan_BMS_CmdOpenBalan() == 2.0));

  /* Outport: '<Root>/OutCmdCloseBalan' incorporates:
   *  Constant: '<S12>/Constant'
   *  DataTypeConversion: '<S6>/Data Type Conversion2'
   *  Inport: '<Root>/BMS_CmdOpenBalan'
   *  Logic: '<S6>/Logical Operator'
   *  RelationalOperator: '<S12>/Compare'
   */
  Rte_IWrite_Runnable_Step_OutCmdCloseBalan_OutCmdCloseBalan(rtb_Compare_p &&
    (Rte_IRead_Runnable_Step_BMS_CmdOpenBalan_BMS_CmdOpenBalan() == 1.0));

  /* Outport: '<Root>/OutCmdCloseDSG' incorporates:
   *  Constant: '<S16>/Constant'
   *  DataTypeConversion: '<S8>/Data Type Conversion6'
   *  Inport: '<Root>/BMS_CmdOpenDSG'
   *  Logic: '<S8>/Logical Operator'
   *  RelationalOperator: '<S16>/Compare'
   */
  Rte_IWrite_Runnable_Step_OutCmdCloseDSG_OutCmdCloseDSG(rtb_Compare_p &&
    (Rte_IRead_Runnable_Step_BMS_CmdOpenDSG_BMS_CmdOpenDSG() == 1.0));

  /* Logic: '<S7>/Logical Operator1' incorporates:
   *  Constant: '<S13>/Constant'
   *  Inport: '<Root>/BMS_CmdOpenCHG'
   *  RelationalOperator: '<S13>/Compare'
   */
  rtb_LogicalOperator1_gr = (rtb_Compare_p &&
    (Rte_IRead_Runnable_Step_BMS_CmdOpenCHG_BMS_CmdOpenCHG() == 2.0));

  /* DataTypeConversion: '<S9>/Data Type Conversion7' incorporates:
   *  Constant: '<S18>/Constant'
   *  Inport: '<Root>/current'
   *  Logic: '<S9>/Logical Operator'
   *  RelationalOperator: '<S18>/Compare'
   */
  rtb_LookupTable = (rtb_LogicalOperator1_gr &&
                     (Rte_IRead_Runnable_Step_current_current() >= 0.0));

  /* Logic: '<S8>/Logical Operator1' incorporates:
   *  Constant: '<S15>/Constant'
   *  Inport: '<Root>/BMS_CmdOpenDSG'
   *  RelationalOperator: '<S15>/Compare'
   */
  rtb_Compare_p = (rtb_Compare_p &&
                   (Rte_IRead_Runnable_Step_BMS_CmdOpenDSG_BMS_CmdOpenDSG() ==
                    2.0));

  /* Switch: '<S9>/Switch' incorporates:
   *  Constant: '<S18>/Constant'
   *  Inport: '<Root>/current'
   *  Logic: '<S9>/Logical Operator'
   *  RelationalOperator: '<S18>/Compare'
   */
  if (!((rtb_LogicalOperator1_gr && (Rte_IRead_Runnable_Step_current_current() >=
         0.0)) > 0)) {
    /* Switch: '<S9>/Switch1' incorporates:
     *  Constant: '<S17>/Constant'
     *  Constant: '<S9>/Constant'
     *  Constant: '<S9>/Constant1'
     *  Logic: '<S9>/Logical Operator1'
     *  RelationalOperator: '<S17>/Compare'
     */
    if (((Rte_IRead_Runnable_Step_current_current() <= 0.0) && rtb_Compare_p) >
        0) {
      rtb_LookupTable = 2.0;
    } else {
      rtb_LookupTable = 3.0;
    }

    /* End of Switch: '<S9>/Switch1' */
  }

  /* End of Switch: '<S9>/Switch' */

  /* Outport: '<Root>/Batt_Fault' */
  Rte_IWrite_Runnable_Step_Batt_Fault_Batt_Fault(rtb_LookupTable);

  /* Outport: '<Root>/OutCmdOpenDSG' incorporates:
   *  DataTypeConversion: '<S8>/Data Type Conversion5'
   */
  Rte_IWrite_Runnable_Step_OutCmdOpenDSG_OutCmdOpenDSG(rtb_Compare_p);

  /* Outport: '<Root>/OutCmdOpenCHG' incorporates:
   *  DataTypeConversion: '<S7>/Data Type Conversion3'
   */
  Rte_IWrite_Runnable_Step_OutCmdOpenCHG_OutCmdOpenCHG(rtb_LogicalOperator1_gr);

  /* Outport: '<Root>/OutCurrent' incorporates:
   *  Inport: '<Root>/current'
   */
  Rte_IWrite_Runnable_Step_OutCurrent_OutCurrent
    (Rte_IRead_Runnable_Step_current_current());

  /* Update for Delay: '<S4>/Delay' incorporates:
   *  Gain: '<S4>/Gain2'
   */
  rtDW.icLoad = 0U;
  rtDW.Delay_DSTATE = 0.9 * rtb_Add_i;

  /* Update for Delay: '<S1>/Delay' */
  rtDW.icLoad_o = 0U;
  rtDW.Delay_DSTATE_m = rtb_Switch;

  /* Update for Delay: '<S10>/Delay' incorporates:
   *  Constant: '<S10>/Constant1'
   *  Delay: '<S10>/Delay1'
   *  Product: '<S10>/Product'
   *  Sum: '<S10>/Add'
   */
  rtDW.Delay_DSTATE_b = (0.1 + rtDW.Delay_DSTATE_b) * rtDW.Delay1_DSTATE;

  /* Update for Delay: '<S3>/Delay' */
  rtDW.icLoad_j = 0U;

  /* Update for Delay: '<S10>/Delay1' */
  rtDW.Delay1_DSTATE = rtb_DataTypeConversion2;
}

/* Model initialize function */
void Runnable_Init(void)
{
  /* InitializeConditions for Delay: '<S4>/Delay' */
  rtDW.icLoad = 1U;

  /* InitializeConditions for Delay: '<S1>/Delay' */
  rtDW.icLoad_o = 1U;

  /* InitializeConditions for Delay: '<S3>/Delay' */
  rtDW.icLoad_j = 1U;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
