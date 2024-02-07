#include "uart.h"

char command[commandSize];
uint8_t commandIndex;
bool commandParsed;

volatile unsigned char value;
ISR(USART_RX_vect){
	value = UDR0;
	commandParsed = false;
	command[commandIndex] = value;
	if(value == '\n'){
		commandIndex = 0;
		memset(command, 0, sizeof(command));
		return;
	}
	if(commandIndex < 15){
		commandIndex++;
	}else{
		commandIndex = 0;
		memset(command, 0, sizeof(command));
	}
}

void setupUart(uint32_t freq_cpu, uint32_t baudRate){
	commandIndex = 0;
	//enable tx
	uint16_t UBRR0_value = (freq_cpu/(baudRate*8))-1;
	UCSR0A |= 1<<U2X0;
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0) | (1<<RXCIE0);
	UBRR0H = (uint8_t)(UBRR0_value) >> 8;
	UBRR0L = (uint8_t)UBRR0_value;
}

void sendByte(uint8_t u8Data){
	while(!(UCSR0A&(1<<UDRE0))){};
	UDR0 = u8Data;
}
void sendStr(char* str){
	for (uint8_t i = 0; i < strlen(str); i++){
		sendByte(str[i]);
	}
}
void parseCommand(){

}