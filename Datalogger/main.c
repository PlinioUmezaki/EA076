/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "AS1.h"
#include "ASerialLdd1.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "WAIT1.h"
#include "KSDK1.h"
#include "Bit1.h"
#include "BitIoLdd1.h"
#include "Bit4.h"
#include "BitIoLdd2.h"
#include "GI2C1.h"
#include "CI2C1.h"
#include "Bit7.h"
#include "BitIoLdd7.h"
#include "UTIL1.h"
#include "UTIL2.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "Bit3.h"
#include "BitIoLdd3.h"
#include "Bit2.h"
#include "BitIoLdd4.h"
#include "Bit5.h"
#include "BitIoLdd5.h"
#include "Bit6.h"
#include "BitIoLdd6.h"
#include "PTC.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* User includes (#include below this line is not maintained by Processor Expert) */

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */

int main(void)

{

  PTC_Init(); // Inicializacao dos pinos do PORTC (Habilita resistores pull-up internos caso necessario)

  PE_low_level_init();

  
  
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();
  #endif
 
  for(;;){}

}




