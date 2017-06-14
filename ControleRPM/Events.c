/* ###################################################################
**     Filename    : Events.c
**     Project     : Controle RPM
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-05-22, 16:13, # CodeGen: 0
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

/* Buffer que armazena conteudo para enviar para tela atraves da UART */
typedef struct {
  char data[10];
  unsigned int tam_buffer;
} serial_buffer;
serial_buffer Buffer; 

/* Adiciona caractere ao buffer */
int buffer_add(char c_in) {
  if (Buffer.tam_buffer < 10) {
    Buffer.data[Buffer.tam_buffer++] = c_in;
    return 1;
  }
  return 0;
}

/* Limpa buffer */
void buffer_clean() {
  Buffer.tam_buffer = 0;
}

/* Envia caracteres do buffer para a tela atraves da UART.*/
void send_buffer(char* buffer) {
	int i = 0;
    
	// Antes de enviar os caracteres do Buffer, vai para a linha de baixo.
	AS1_SendChar('\n');
	AS1_SendChar('\r');
    
    // Envia os caracteres do buffer para a tela do computador.
	while(buffer[i] != '\0') {
		AS1_SendChar(buffer[i]);
		i++;
	}
	AS1_SendChar('\n');
	i = 0;
}

// Inicializacao das variaveis globais.
int contador = 0;
int rpm = 0;
int rpm_pwm = 0;
int rpm_desejado;

// Funcao que implementa o controle P.
void ajustaRPM(){
	int erro = rpm - rpm_desejado;		// Calculo do erro.
	float alpha = 0.02;					// A constante foi ajustada em 0,02.
	rpm_pwm = rpm_pwm + alpha*erro;		// Calcula valor do PWM que deve ser acrescido (desacelerar) ou decrescido (acelerar).
	PWM1_SetRatio8(rpm_pwm);			// Reconfigura PWM.
}

void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

// A cada vez que o receptor infra-vermelho gera uma interrupcao, o contador é incrementado.
void Receptor_OnInterrupt(void)
{
      contador++;
}

// Interrupcao do timer a cada segundo.
void TI1_OnInterrupt(void)
{
	unsigned char buffer[10];				// Cria-se um buffer.
	Receptor_Disable();						// Desabilita interrupcoes do receptor.
	rpm = contador*30;						// Multiplica-se o contador por 30, pois existem duas hélices.
	contador = 0;							// Retorna o contador ao estado inicial.
	UTIL1_Num32sToStr(buffer, 5, rpm);		// Converte o RPM encontrado para string.
	int i = 0;
	AS1_SendChar('\r');
	// Envia caracteres do RPM.
	while (buffer[i] != '\0'){
		AS1_SendChar(buffer[i]);
		i++;
	}
	ajustaRPM();							// Chama a função de controle para ajuste.
	Receptor_Enable();						// Reabilita interrupcoes do receptor.
}

void AS1_OnError(void)
{
  /* Write your code here ... */
}

void AS1_OnTxChar(void)
{
  /* Write your code here ... */
}

void AS1_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

// Funcao para leitura de caracteres. Utilizada apenas para receber o numero de rotacoes como input.
void AS1_OnRxChar(void)
{
  AS1_Disable();	// Desabiltia interrupcoes da UART.
  char c;
  if(AS1_RecvChar(&c) == ERR_OK){ // Se algum caractere do teclado realmente foi recebido
	  if (c == '\r') {
		  buffer_add('\0');			// Adiciona final de linha no buffer.
		  AS1_ClearRxBuf();			// Limpa buffer da UART.
		  char* input = Buffer.data;
		  UTIL1_ScanDecimal32sNumber(&input, &rpm_desejado);	// Converte RPM desejado para string.
		  buffer_clean();		// Limpa-se o buffer, variavel ja foi atualizada.
		  AS1_SendChar('\r');	// Retorno de linha.
		  AS1_SendChar('\n');	// Pula para a proxima linha.
		  TI1_Enable();			// Habilita timer.
		  Receptor_Enable();	// Habilita interrupcoes do receptor.
	  } else {
		  buffer_add(c);		// Adiciona ao buffer caso um enter ainda nao tenha sido pressioando.
	  }
  } 
  AS1_SendChar(c);				// Envia caracter digitado para a tela.
  AS1_Enable();					// Reabilita interrupcao da UART.
}

void AS1_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

#ifdef __cplusplus
}  /* extern "C" */
#endif 