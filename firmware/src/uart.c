#include "uart.h"

char uart_command[UART_BUFFER_SIZE];
bool uart_commandParsed; //set to 0 whenever something is received

uint8_t uart_bufferIndex = 0;
char uart_buffer[UART_BUFFER_SIZE];
volatile char value;

ISR(USART_RX_vect){
	value = UDR0;
	uart_buffer[uart_bufferIndex] = value;
	if(value == LINE_TERMINATOR){ // command is fully received
		//mark command as unparsed and copy to global uart_command
		uart_commandParsed = false;
		memcpy(uart_command, uart_buffer, UART_BUFFER_SIZE);
		//clean buffer and reset index
		memset(uart_buffer, 0, UART_BUFFER_SIZE);
		uart_bufferIndex = 0;
		return;
	}
	if(uart_bufferIndex < (UART_BUFFER_SIZE - 1)){
		uart_bufferIndex++;
	}else{ // oh oh! clear buffer D: ? should I send a warn?
		sendByte(201);
		sendByte(uart_buffer[uart_bufferIndex]);
		sendByte('\n');
		uart_bufferIndex = 0;
		memset(uart_buffer, 0, UART_BUFFER_SIZE);
	}
}

void setupUart(uint32_t freq_cpu, uint32_t baudRate){
	//enable tx
	UCSR0A |= 1 << U2X0; // enable double speed
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0) | (1<<RXCIE0); // enable rx, tx anad rx interrupts

	uint16_t UBRR0_value = (freq_cpu/(baudRate*8))-1; //c alculate baudRate
	UBRR0H = (uint8_t)(UBRR0_value) >> 8; //set baudRate
	UBRR0L = (uint8_t)UBRR0_value;
}

void sendByte(uint8_t u8Data){
	while(!(UCSR0A&(1<<UDRE0))){}; // wait until ready to send
	UDR0 = u8Data; //send data
}
void sendStr(char* str){
	for (uint8_t i = 0; i < strlen(str); i++){
		sendByte(str[i]);
	}
}
