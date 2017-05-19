/* ###################################################################
**     Filename    : Events.c
**     Project     : Datalogger
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-04-03, 17:16, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BUFFER_SIZE 15

/* Funcoes e Variaveis para o UART */
    
/*Buffer que armazena conteudo para enviar para tela através do UART */
typedef struct {
  char data[MAX_BUFFER_SIZE];
  unsigned int tam_buffer;
} serial_buffer;
serial_buffer Buffer; 


char out_buffer[15];



/* Limpa buffer */
void buffer_clean() {
  Buffer.tam_buffer = 0;
}

/* Adiciona caractere ao buffer */
int buffer_add(char c_in) {
  if (Buffer.tam_buffer < MAX_BUFFER_SIZE) {
    Buffer.data[Buffer.tam_buffer++] = c_in;
    return 1;
  }
  return 0;
}

/* Envia caracteres do buffer para a tela através do UART.*/
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

/* Variaveis e Funcoes do Teclado Matricial
 * 
 */

// Variaveis globais para debouncing
int debouncing_mode = 0;
int debouncing_counter;
    
// Variavel que armazena o caractere retornado pelo Janio Quadros
char character;
    
char janioQuadros(){
	/* Funcao para varredura (pegou a referencia?) do teclado matricial:
	 * Bits de 1 a 3: colunas 1, 3 e 2, respectivamente;
	 * Bits de 4 a 6: linhas 1, 2 e 4, respectivamente.
	 */ 
	Bit4_SetVal();
	Bit5_SetVal();
	Bit6_SetVal();
	// Envia sinal 0 para primeira linha e percorre colunas em busca de outro 0.
	Bit4_ClrVal();
	if(Bit1_GetVal() == 0) {
		return '1';
	}
	if(Bit2_GetVal() == 0) {
		return '2';
	}
	if(Bit3_GetVal() == 0) {
		return '3';
	}
	Bit4_SetVal();
	// Envia sinal 0 para segunda linha e percorre colunas em busca de outro 0.
	Bit5_ClrVal();
	if(Bit1_GetVal() == 0) {
		return '4';
	}
	if(Bit2_GetVal() == 0) {
		return '5';
	}
	if(Bit3_GetVal() == 0) {
		return '6';
	}
	Bit5_SetVal();
	// Envia sinal 0 para terceira linha e percorre colunas em busca de outro 0.
	Bit6_ClrVal();
	if(Bit1_GetVal() == 0) {
		return '*';
	}
	if(Bit2_GetVal() == 0) {
		return '0';
	}
	if(Bit3_GetVal() == 0) {
		return '#';
	}
	Bit6_SetVal();
	return '\0';	// Retorna "fim de string" em caso de insucesso
}

/* Variaveis globais para a medicao com sensor */
// Variavel que armazena as medidas feitas pelo sensor
byte measure;
    
// Variaveis de delay para desacelerar a medicao automatica.
int measuring_counter;
int measuring_mode = 1; // Modo de medicao inicial como ativo

    
// Flag para o pisca led
int flag_led = 0;

// Flag para ativar e desativar a medicao automatica
int flag_continuous_recording = 0;

//Flag para ativar e desativar a gravacao das medidas na memoria.
int flag_record;


/* Funcoes e Variaveis para EEPROM */

/* OBS: para a leitura e escrita na memoria EEPROM, eh utilizado um sistema de arquivos.
 * Na primeira posicao da memoria eh armazenada o numero de elementos existentes na memoria.
 * Cada vez que algum elemento a memoria eh gravada o valor da primeira posicao eh atualizada 
 * incrementando-a.
 */

//Memoria EEPROM
unsigned char EEPROM[100];

// Funcao que le os valores da EEPROM
unsigned char read_byte (int posicao){
	return EEPROM[posicao];
}

// Funcao que escreve os valores da EEPROM
void write_byte (int posicao, unsigned char dado){
	unsigned char ocupacao = read_byte(0);
	ocupacao++;
	if(ocupacao <= 100) {
		EEPROM[ocupacao] = dado;
		EEPROM[0] = ocupacao; // Atualizacao do numero de elementos na EEPROM.
	}
}

// Funcao que retorna o numero de elementos na EEPROM, armazenada na primeira posicao.
unsigned char return_N() {
	return EEPROM[0];
}

// Funcao que remove todos os valores armazenados na EEPROM, inserindo zero na primeira posicao.
void clear_EEPROM() {
	EEPROM[0] = 0;
}
    
    

void AS1_OnRxChar(void)
{
    
    /* Interrupcao executada quando UART receber caracteres do teclado */
    
    AS1_Disable(); // Desabilita interrupcao para a verificacao do caractere.
    char c;
    if(AS1_RecvChar(&c) == ERR_OK){ // Se algum caractere do teclado realmente foi recebido
        if (c == '\r') { // Condicao quando foi pressionada a tecla ENTER
            buffer_add('\0'); /* Se recebeu um fim de linha, coloca um terminador de string no buffer */
            AS1_ClearRxBuf();
            // Caso receba o comando "ID", retorna uma string de identificacao
            if(strcmp(Buffer.data, "ID") == 0) {
                sprintf(out_buffer, "%s\r\n", "ALO TIAGAO");
                send_buffer(out_buffer);
            }
            // Caso receba o comando "PING", responde de volta "PONG"
            if(strcmp(Buffer.data, "PING") == 0) {
                byte dado;
                sprintf(out_buffer, "%s\r\n", "PONG");
                send_buffer(out_buffer);
            }
            // Caso receba o comando "MEASURE", responde de volta a medida do sensor no momento da chamada
            if(strcmp(Buffer.data, "MEASURE") == 0) {
                AD1_Measure(0);
                flag_record = 0;
            }
            // Caso receba o comando "MEMSTATUS", responde de volta a quantidade de elementos na EEPROM
            if(strcmp(Buffer.data, "MEMSTATUS") == 0) {
                sprintf(out_buffer, "%d\r\n", return_N());
                send_buffer(out_buffer);
            }
            // Caso receba o comando "RESET", formata a EEPROM
            if(strcmp(Buffer.data, "RESET") == 0) {
                clear_EEPROM();
                send_buffer(""); // Pula para linha de baixo na tela.
            // Caso receba o comando "RECORD", mede e grava na EEPROM no momento da chamada.
            if(strcmp(Buffer.data, "RECORD") == 0) {
                flag_record = 1;
                AD1_Measure(0);
            }
            /* Caso receba o comando "GET N", sendo N a posicao da memoria que se deseja acessar,
             * acessa o elemento da posicao N da EEPROM e retorna na tela o valor armazenado
             */
            if(strncmp(Buffer.data, "GET ", 4) == 0) {
                char dado;
                char* elemento = Buffer.data;
                uint8_t posicao;
                elemento += 4; // desloca 4 caracteres (bytes) em relacao ao endereco do Buffer para pegar o N no formato de string.
                UTIL1_ScanDecimal8uNumber(&elemento, &posicao); // Converte o N para valor de 8 bits sem sinal.
                if(posicao <= return_N()){ // Verifica se N eh um valor abaixo do tamanho atual da EEPROM
                    dado = read_byte(posicao); // Leitura da EEPROM
                    sprintf(out_buffer, "%d\r\n", dado);
                    send_buffer(out_buffer);
                } else {
                    // Se N for maior que a quantidade de elementos armazenados na EEPROM, simplesmente ignora e pula para próxima linha na tela.
                    send_buffer("");
                }  
            }
            buffer_clean(); // Limpeza do buffer de UART apos leitura do comando.
        } else {
            buffer_add(c); // Caso nao esteja pressionando o ENTER simplemente adiciona o caracter recebido pelo UART no buffer
        }
    }
    AS1_SendChar(c); // Envio de caracter recebido pelo teclado na tela.
    AS1_Enable(); // Reabilita interrupcao de UART
}

/* Interrupcao que executa apos o sensor medir algum valor */
void AD1_OnEnd(void)
{
    
	AD1_GetValue8(&measure); // Pega o valor medido
    // Se o flag de medicao automatica for ativada, nao envia as medidas a tela do comoputador
	if (flag_continuous_recording == 0){
		sprintf(out_buffer, "%d\r\n", measure);
		send_buffer(out_buffer);
	}
    // Caso o flag de gravacao (ativada pelo comando RECORD ou pela medicao automatica), estiver habilitada, chama a funcao de gravacao na memoria.
	if(flag_record == 1) {
		write_byte(0, measure);
	}
	
	
}

void TI1_OnInterrupt(void)
{
	/* Interrupcao periodica para:
     * Varredura e envio de caracteres do teclado matricial;
     * Medicao automatica do sensor;
     * Piscagem de LEDs.
	 */
	
    /* Varredura do teclado teclado matricial: O conceito de debouncing eh trabalhado
     * por meio de um contador e uma flag.
     */
    
	// Caso a flag seja falsa, Janio Quadros vira com a sua vassourinha (pegou a referencia?)
	if (debouncing_mode == 0) {
		character = janioQuadros();
		// Caso um caracter valido tenha sido lido:
		if(character != 0){
			  debouncing_counter = 5000;	// Configura contador
			  debouncing_mode = 1;			// Ativa a flag
			  if(character == '*') {
				  AS1_SendChar(character); // Envio do character '*' para a tela do computador
				  int length = strlen(Buffer.data);
				  buffer_add('\0');
				  if (Buffer.data[strlen(Buffer.data) - 2] == '#') { // Verifica se o primeiro caractere do comando eh '#'.
                      
                      // Caso comando seja "#1*", LED pisca.
					  if(Buffer.data[strlen(Buffer.data) - 1] == '1') {
						  flag_led = 6; // Flag de pisca led ativo
						  send_buffer(""); // Pula para a proxima linha na tela do PC
					  }
                      // Caso comando seja "#2*", grava o valor do sensor na EEPROM (mesma acao do comando RECORD do teclado.
					  if(Buffer.data[strlen(Buffer.data) - 1] == '2') {
						  flag_record = 1; // Flag de gravacao ativo
                          AD1_Measure(0);
					  }
                      // Caso comando seja "#3*", eh ativada a medicao automatica
					  if(Buffer.data[strlen(Buffer.data) - 1] == '3') {
						  flag_continuous_recording = 1; // Flag de medicao automatica ativa
						  send_buffer(""); // Pula para a proxima linha na tela do PC
					  }
                      // Caso comando seja "#4*", eh desativada a medicao automatica
					  if(Buffer.data[strlen(Buffer.data) - 1] == '4') {
						  flag_continuous_recording = 0; // Flag de medicao automatica desabilitada.
						  send_buffer(""); // Pula para a proxima linha na tela do PC
					  }
				  }
				  buffer_clean(); // Limpeza do buffer apos ler comando
			  }else {
			  	  buffer_add(character);    //Simplesmente armazena no buffer caso nao pressionar '*'
			  	  AS1_SendChar(character);		// Envia caracter recebido
			  }
			  
		  }
	}
	// Caso a flag seja verdadeira, o contador eh decrementado
	else {
		debouncing_counter--;
		// Ao atingir 0, a flag se torna falsa novamente
		if(debouncing_counter == 0){
		  debouncing_mode = 0;
	   }
    }
    
	/* Se o flag de pisca LED esteja ativado, o valor de saida da placa conectada ao LED eh negada varias
     * vezes. Para cada operacao de negacao, eh gerado um delay para que o LED nao pisque tao rapido a ponto
     * de nao se tornar visivel tal comportamento.
     */
	if (flag_led > 0){
		Bit7_NegVal(); //operacao de negacao na saida
		int i = 1200000; //delay
		for (i; i > 0; i--);
		flag_led--; // decremento do flag.
	}
	
    /* Se a medicao automatica esteja ativada, entra no modo de medicao e mede e grava na EEPROM de maneira
     * continua. Para cada operacao de medicao e gravacao feita, fica algum tempo inativo para
     * nao provocar medicoes excessivas ao ponto de estourar a EEPROM. 
     * O mecanismo de delay eh o mesmo utilizado no debouncing do teclado matricial.
     */
	if (flag_continuous_recording == 1){
		if (measuring_mode == 1) { // Se estiver no modo de medicao
			flag_record = 1; // Ativa o modo de gravacao
			AD1_Measure(0); // Medicao do sensor.
			measuring_counter = 15000;
			measuring_mode = 0; // Desativa o modo de medicao
		}
		else {
			measuring_counter--; // Decremento do contador
			if (measuring_counter == 0) { // Caso o contador se torne zero, volta para o modo de medicao
				measuring_mode = 1;
			}
		}
	}
}

void Cpu_OnNMIINT(void)
{
    
}

#ifdef __cplusplus
}  /* extern "C" */
#endif 
