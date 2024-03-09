#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 4

extern char command[BUFFER_SIZE];
extern bool commandParsed; //set to 0 whenever something is received


void setupUart(uint32_t freq_cpu, uint32_t baudRate);

void sendByte(uint8_t u8Data);
void sendStr(char* str);