/* ###################################################################
**     Filename    : Events.c
**     Project     : Semaforo
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-03-13, 16:54, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
      
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Declaracao das variaveis globais */

byte i = 0; // Valor do ADC (intensidade luminosa).

int state = 0;
int counter = 0;


/* ===================================================================*/
void Cpu_OnNMIINT(void)
{
}

/* Interrupcao do Timer */
void TI1_OnInterrupt(void)
{
  AD1_Measure(0);  // Leitura do ADC
  /* Se i for maior que 220, significa que esta de noite. */
  if (i > 220) {
	  EInt1_Disable(); // Desabilita interrupcao do botao.
	  /* Desliga os Leds Vermelhos e Verdes */
	  Bit1_ClrVal(); 
	  Bit2_ClrVal();
	  Bit3_ClrVal();
	  Bit5_ClrVal();
	  state = 5; // Leva a maquina de estados para o ultimo caso.
	  counter = 0; // Mantem o contador em zero.
  }
  /* Caso esteja de dia */
  else {
	  /* Se o estado for 5 */
	  if(state == 5) {
		  EInt1_Enable(); // Habilita interrupcao do botao.
	  	  state = 0; // Reinicializa a maquina de estado.
	  }
  }
  /* Se estiver no estado permanente (contador nulo) ou o estado nao permanente teve o contador estourado */
  if (counter == 0) {
	  /* maquina de estados permanentes*/
	  switch(state) {
	  /* estado inicial: carros liberados (sinal verde) e pedestre impedido (sinal vermelho) */
	  case 0:
		  Bit1_ClrVal();
		  Bit2_SetVal();
		  Bit3_SetVal();
		  Bit4_ClrVal();
		  Bit5_ClrVal();
		  break;
	  /* estado 1: sinal amarelo (alerta) para carros por 5 segundos */
	  case 1:
		  EInt1_Disable(); // Desabilita interrupcao do botao.
		  Bit3_NegVal();
		  Bit4_NegVal();
		  counter = 5;
		  state = 2;
		  break;
	  /* estado 2: sinal liberado para os pedestres e impedido para carros por 10 segundos */ 
	  case 2:
		  Bit1_NegVal();
		  Bit2_NegVal();
		  Bit4_NegVal();
		  Bit5_NegVal();
		  counter = 10;
		  state = 3;
		  break;
	  /* estado 5: estado quando esta noite, apenas pisca o Led amarelo. */
	  case 5:
		  Bit4_NegVal();
		  break;
	  default:
		  break;
	  }
  }
  else {
	  counter--;  // decrementa o contador em estados nao permanentes.
	  /* Maquina de estados nao permanentes. */
	  switch(state) {
	   /* estado 3: estado de transicao de estado 2 para estado 4. */
	   case 3:
		   if (counter == 0) {
			   state = 4;
			   counter = 5;
		   }
		   break;
	   /* estado 4: sinal fechado para carros e sinal verde piscante para os pedestres. */
	   case 4:
		   Bit1_NegVal();
		   if (counter == 0) {
			   EInt1_Enable(); // Habilita interrupcao do botao.
			   state = 0;
		   }
		   break;
		   
	  }
  }
}

/* Interrupcao do botao: ao ser acionado a interrupcao, o sistema passa para o estado 1 apenas após 3 segundos. */
void EInt1_OnInterrupt(void)
{
	state = 1;
	counter = 3;
}

void AD1_OnEnd(void)
{
	AD1_GetValue8(&i);
}

void AD1_OnCalibrationEnd(void)
{
	
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

