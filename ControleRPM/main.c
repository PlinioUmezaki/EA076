/* ###################################################################
**     Filename    : main.c
**     Project     : Controle RPM
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-05-22, 16:13, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "PWM1.h"
#include "PwmLdd1.h"
#include "TU1.h"
#include "Ponte_H1.h"
#include "BitIoLdd1.h"
#include "Ponte_H2.h"
#include "BitIoLdd2.h"
#include "Receptor.h"
#include "Receptor.h"
#include "ExtIntLdd1.h"
#include "Bits1.h"
#include "BitsIoLdd1.h"
#include "Bits2.h"
#include "BitsIoLdd2.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TU2.h"
#include "AS1.h"
#include "ASerialLdd1.h"
#include "UTIL1.h"
#include "KSDK1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

int main(void)
{
  PE_low_level_init();

  // Inicializa PWM, ponte H e LEDs.
  Ponte_H2_SetVal();
  Ponte_H1_ClrVal();
  PWM1_SetRatio8(0);
  Bits1_SetBit(0);
  Bits1_SetBit(1);
  Bits1_SetBit(2);
  Bits2_SetBit(0);
  Bits2_SetBit(1);
  
  // Interrupções são desabilitadas até que o input seja fornecido.
  TI1_Disable();
  Receptor_Disable();
  
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  
  #endif

  for(;;){}
} 
