#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "uart.h"
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#define MAX_GAIN ((double) 1.077777)
#define MIN_GAIN ((double) 0.911112)

char getHex(uint8_t num);
//sizes: gstr[5]
double getgain(const uint8_t *gstr);
uint8_t getNDigit(double gain, uint8_t digitNumber);
//sizes: gainString[5]
void encode_gain(uint8_t *gainString, double gain);

bool getByteParity(uint8_t byte_);
//sizes: parityString[4], dataString[12]
void getParity(uint8_t *parityString, uint8_t *dataString);