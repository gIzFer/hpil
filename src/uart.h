#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define commandSize 16
extern char command[commandSize];
extern uint8_t commandIndex;
extern bool commandParsed; //set to 0 whenever something is received


void setupUart(uint32_t freq_cpu, uint32_t baudRate);

void sendByte(uint8_t u8Data);
void sendStr(char* str);
void parseCommand();