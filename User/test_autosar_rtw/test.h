/*
 * File: test.h
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

#ifndef RTW_HEADER_test_h_
#define RTW_HEADER_test_h_
#include <math.h>
#ifndef test_COMMON_INCLUDES_
# define test_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "Rte_test.h"
#endif                                 /* test_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */

/* Block signals and states (default storage) for system '<Root>' */
typedef struct tag_DW {
  real_T Output1;                      /* '<S10>/count' */
  real_T Delay_DSTATE;                 /* '<S4>/Delay' */
  real_T Delay_DSTATE_m;               /* '<S1>/Delay' */
  real_T Delay_DSTATE_b;               /* '<S10>/Delay' */
  real_T Delay_DSTATE_mm;              /* '<S3>/Delay' */
  real_T Delay1_DSTATE;                /* '<S10>/Delay1' */
  uint8_T icLoad;                      /* '<S4>/Delay' */
  uint8_T icLoad_o;                    /* '<S1>/Delay' */
  uint8_T icLoad_j;                    /* '<S3>/Delay' */
} DW;

/* Constant parameters (default storage) */
typedef struct {
  /* Expression: [0,0.014249,0.045337,0.25259,0.48057,0.72668,0.89508,0.9728,1]
   * Referenced by: '<Root>/Lookup Table'
   */
  real_T LookupTable_tableData[9];

  /* Expression: [3.4,3.42,3.45,3.6,3.75,3.9,4.05,4.2,4.25]
   * Referenced by: '<Root>/Lookup Table'
   */
  real_T LookupTable_bp01Data[9];
} ConstP;

/* Block signals and states (default storage) */
extern DW rtDW;

/* Constant parameters (default storage) */
extern const ConstP rtConstP;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'test'
 * '<S1>'   : 'test/Ah_count'
 * '<S2>'   : 'test/Compare To Constant6'
 * '<S3>'   : 'test/LPF'
 * '<S4>'   : 'test/LPF1'
 * '<S5>'   : 'test/Subsystem'
 * '<S6>'   : 'test/Subsystem1'
 * '<S7>'   : 'test/Subsystem2'
 * '<S8>'   : 'test/Subsystem3'
 * '<S9>'   : 'test/Subsystem4'
 * '<S10>'  : 'test/standstill'
 * '<S11>'  : 'test/Subsystem1/Compare To Constant'
 * '<S12>'  : 'test/Subsystem1/Compare To Constant1'
 * '<S13>'  : 'test/Subsystem2/Compare To Constant2'
 * '<S14>'  : 'test/Subsystem2/Compare To Constant3'
 * '<S15>'  : 'test/Subsystem3/Compare To Constant4'
 * '<S16>'  : 'test/Subsystem3/Compare To Constant5'
 * '<S17>'  : 'test/Subsystem4/Compare To Constant'
 * '<S18>'  : 'test/Subsystem4/Compare To Constant1'
 * '<S19>'  : 'test/standstill/Compare To Constant'
 * '<S20>'  : 'test/standstill/count'
 */
#endif                                 /* RTW_HEADER_test_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
