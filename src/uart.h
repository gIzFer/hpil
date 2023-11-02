#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>


void setupUart(uint32_t freq_cpu, uint32_t baudRate);

void sendByte(uint8_t u8Data);
void sendStr(char* str);
