/* ###################################################################
**     Filename    : main.c
**     Project     : Semaforo
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-03-13, 16:54, # CodeGen: 0
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
#include "Bit1.h"
#include "BitIoLdd1.h"
#include "Bit2.h"
#include "BitIoLdd2.h"
#include "Bit3.h"
#include "BitIoLdd3.h"
#include "Bit4.h"
#include "BitIoLdd4.h"
#include "Bit5.h"
#include "BitIoLdd5.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "EInt1.h"
#include "ExtIntLdd1.h"
#include "AD1.h"
#include "AdcLdd1.h"

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"


int main(void)

{

  PE_low_level_init();

  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif

  for(;;){}

} 


