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
         
/* MODULE main */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Ponte_H1.h"
#include "BitIoLdd1.h"
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
#include "Bit1.h"
#include "BitIoLdd3.h"
#include "TI2.h"
#include "TimerIntLdd2.h"
#include "TU1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

int main(void)
{
  /* Write your local variable definition here */

  PE_low_level_init();
  
  // Desabilita todas as interrupcoes.
  TI1_Disable();
  TI2_Disable();
  Receptor_Disable();
  
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  
  #endif

  for(;;){}
} 