#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define UART_BUFFER_SIZE 16
#define LINE_TERMINATOR '\n'

//when we receive a command from serial its stored here and its marked as unparsed
extern char uart_command[UART_BUFFER_SIZE];
extern bool uart_commandParsed; //set to 0 (false) whenever something is received


void setupUart(uint32_t freq_cpu, uint32_t baudRate);

void sendByte(uint8_t u8Data);
void sendStr(char* str);