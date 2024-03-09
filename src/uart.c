#include "uart.h"

char buffer[BUFFER_SIZE];
uint8_t bufferIndex;
char command[BUFFER_SIZE];
bool commandParsed;

volatile unsigned char value;
ISR(USART_RX_vect){
	value = UDR0;
	buffer[bufferIndex] = value;
	if(value == '\n'){
		commandParsed = false;
		memcpy(command, buffer, BUFFER_SIZE);
		memset(buffer, 0, BUFFER_SIZE);
		bufferIndex = 0;
		return;
	}
	if(bufferIndex < (BUFFER_SIZE - 1)){
		bufferIndex++;
	}else{
		bufferIndex = 0;
		memset(buffer, 0, BUFFER_SIZE);
	}
}

void setupUart(uint32_t freq_cpu, uint32_t baudRate){
	bufferIndex = 0;
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
