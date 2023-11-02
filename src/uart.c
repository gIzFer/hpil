#include "uart.h"

void setupUart(uint32_t freq_cpu, uint32_t baudRate){
	//enable tx
	uint16_t UBRR0_value = (freq_cpu/(baudRate*8))-1;
	UCSR0A |= 1<<U2X0;
	UCSR0B |= 1<<TXEN0;
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
