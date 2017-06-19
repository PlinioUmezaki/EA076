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

// Inicializacao das variaveis globais.
int flag_start;
int contagem_regressiva = 5;
int contador = 0;
int numero_voltas = 0;
int voltas_completadas = 0;
int tempo_total = 0;
int tempo_apito = 0;

typedef struct {
  int volta;
  int tempo;
} melhor_volta;

melhor_volta best_lap; 

/*Buffer que armazena conteudo para enviar para tela atraves da UART */
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

// Funcao para converter o tempo do contador para o formato minutos.segundos.milissegundos
void converteTempo(){
	int minutos = contador/60000;	// Obtem a quantidade de minutos.
	contador = contador%60000;		// Atualiza contador para resto da divisao.
	int segundos = contador/1000;	// Obtem a quantidade de segundos.
	contador = contador%1000;		// Atualiza contador para resto da divisao.
	sprintf(Buffer.data, "Volta %d -> %d:%d.%d", voltas_completadas, minutos, segundos, contador);	// Converte a informacao para uma string.
	send_buffer(Buffer.data);		// Envia para UART.
	buffer_clean();					// Limpa buffer.
}

// Funcao para converter o tempo da melhor volta.
void melhorVolta(){
	int minutos = best_lap.tempo/60000;		// Obtem a quantidade de minutos.
	best_lap.tempo = best_lap.tempo%60000;	// Atualiza tempo para resto da divisao.
	int segundos = best_lap.tempo/1000;		// Obtem a quantidade de segundos.
	best_lap.tempo = best_lap.tempo%1000;	// Atualiza tempo para resto da divisao.
	sprintf(Buffer.data, "Melhor volta -> %d (%d:%d.%d)", best_lap.volta, minutos, segundos, best_lap.tempo);	// Converte a informacao para uma string.
	send_buffer(Buffer.data);				// Envia para UART.
	buffer_clean();							// Limpa buffer.
}

// Funcao para converter o tempo total.
void tempoTotal(){
	int tempo = tempo_total;				// Cria uma variavel local para nao comprometer tempo medio.
	int minutos = tempo/60000;				// Obtem a quantidade de minutos.
	tempo = tempo%60000;					// Atualiza tempo para resto da divisao.
	int segundos = tempo/1000;				// Obtem a quantidade de segundos.
	tempo = tempo%1000;						// Atualiza tempo para resto da divisao.
	sprintf(Buffer.data, "Tempo total -> %d:%d.%d", minutos, segundos, tempo);	// Converte a informacao para uma string.
	send_buffer(Buffer.data);				// Envia para UART.
	buffer_clean();							// Limpa buffer.
}

// Funcao para calcular e converter o tempo medio.
void tempoMedio(){
	int tempo = tempo_total / numero_voltas;	// Calcula o tempo medio.
	int minutos = tempo/60000;					// Obtem a quantidade de minutos.
	tempo = tempo%60000;						// Atualiza tempo para resto da divisao.
	int segundos = tempo/1000;					// Obtem a quantidade de segundos.
	tempo = tempo%1000;							// Atualiza tempo para resto da divisao.
	sprintf(Buffer.data, "Tempo medio -> %d:%d.%d", minutos, segundos, tempo);	// Converte a informacao para uma string.
	send_buffer(Buffer.data);					// Envia para UART.
	buffer_clean();								// Limpa buffer.
}

/* Envia caracteres do buffer para a tela atraves da UART.*/
void send_buffer(char* buffer) {
	TI1_Disable();
	int i = 0;
    
	// Antes de enviar os caracteres do Buffer, vai para a linha de baixo.
	AS1_SendChar('\n');
	AS1_SendChar('\r');
    
    // Envia os caracteres do buffer para a tela do computador.
	while(buffer[i] != '\0') {
		AS1_SendChar(buffer[i]);
		i++;
	}
	i = 0;
	TI1_Enable();
}

void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

// A cada vez que o receptor infra-vermelho gerar uma interrupcao, significa que uma volta foi completada.
void Receptor_OnInterrupt(void)
{
      voltas_completadas++;				// Incrementa contador de voltas.
      tempo_total += contador;			// Incrementa tempo total.
      if (contador < best_lap.tempo){	// Verifica se o melhor tempo foi batido e faz a atualizacao se necessario.
    	  best_lap.tempo = contador;
    	  best_lap.volta = voltas_completadas;
      }
      
	  converteTempo();					// Converte e imprime o tempo na tela.
      
	  // Caso seja a ultima volta, o apito soa por 1 segundo.
      if (voltas_completadas == numero_voltas - 1){
    	  tempo_apito = 1;
    	  Bit1_SetVal();
    	  TI2_Enable();					// Interrupcao periodica de 1 segundo.
      }
      
	  // Caso todas as voltas tenham sido completadas, o apito soa por 3 segundos e as estatisticas sao impressas na tela.
      if (voltas_completadas == numero_voltas){
    	  tempo_apito = 3;
    	  Bit1_SetVal();
    	  TI2_Enable();					// Interrupcao periodica de 1 segundo.
    	  tempoTotal();
    	  melhorVolta();
    	  tempoMedio();
      }
      
      contador = 0;						// O contador e reiniciado.
}

// Interrupcao que ocorre a cada 1 milissegundo. Nela, o contador e incrementado.
void TI1_OnInterrupt(void)
{
	contador++;
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

// Interrupcao de leitura de caracter para input do numero desejado de voltas.
void AS1_OnRxChar(void)
{
  AS1_Disable();					// Desabilita interrupcao da UART.
  char c;
  if(AS1_RecvChar(&c) == ERR_OK){ // Se algum caractere do teclado realmente foi recebido
	  if (c == '\r') {
		  best_lap.tempo = 0xFFFF;	// Inicializa melhor tempo com o maior valor disponivel.
		  buffer_add('\0');			// Adiciona final de linha ao buffer.
		  AS1_ClearRxBuf();			// Limpa buffer da UART.
		  char* input = Buffer.data;
		  UTIL1_ScanDecimal32sNumber(&input, &numero_voltas);	// Converte input em string.
		  buffer_clean();			// Limpa buffer, variavel ja foi atualizada.
		  AS1_SendChar('\r');
		  AS1_SendChar('\n');
		  flag_start = 1;			// Flag de comeco da corrida e ativada.
		  TI2_Enable();				// Habilita interrupcao de 1 segundo.
	  } else {
		  buffer_add(c);			// Caso nao tenha sido pressionada a tecla enter, apenas adiciona o caracter recebido no buffer.
	  }
  } 
  AS1_SendChar(c);					// Envia caracter digitado para a tela.
  AS1_Enable();						// Reabilita interrupcao da UART.
}

void AS1_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

// Interrupcao que ocorre a cada 1 segundo.
void TI2_OnInterrupt(void)
{
	// Caso em que a flag de comeco da corrida nao esta ativa.
	if(flag_start == 0) {
		// Se o tempo de apito e maior que zero, apenas decrementa.
		if (tempo_apito > 0){
			tempo_apito--;
		}
		// Se o tempo de apito chegou a zero, desabilita interrupcao e desativa o buzzer.
		if (tempo_apito == 0){
			Bit1_ClrVal();
			TI2_Disable();
		}
	// Caso em que a flag de comeco da corrida esta ativa.
	} else {
		// Se a contagem regressiva nao chegou a zero, entra em um switch case para definir qual LED acender.
		if(contagem_regressiva > 0) {
			switch(contagem_regressiva) {
			case 1:
				Bits2_SetBit(1);	// Acende primeiro LED.
				break;
			case 2:
				Bits2_SetBit(0);	// Acende segundo LED.
				break;
			case 3:
				Bits1_SetBit(2);	// Acende terceiro LED.
				break;
			case 4:
				Bits1_SetBit(1);	// Acende quarto LED.
				break;
			case 5:
				Bits1_SetBit(0);	// Acende quinto LED.
				break;
			}
			contagem_regressiva--;	// Decrementa contador.
		  // Quando a contagem regressiva chega a zero, os LEDs são apagados e a flag desativada.
		} else {
			Bits2_ClrBit(1);
			Bits2_ClrBit(0);
			Bits1_ClrBit(2);
			Bits1_ClrBit(1);
			Bits1_ClrBit(0);
			flag_start = 0;
			tempo_apito = 1;	// Configura 1 segundo de apito.
			Bit1_SetVal();		// Ativa o buzzer.
			TI1_Enable();		// Habilita interrupcao a cada 1 milissegundo.
			Receptor_Enable();	// Habilita interrupcao do receptor.
			TI2_Disable();		// Desabiltia interrupcao a cada 1 segundo.
		}
	}
}

#ifdef __cplusplus
}  /* extern "C" */
#endif 