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
double getgain(const uint8_t *gstr);
//void adjusticate(int *digits, bool add, unsigned pos);
//void encode_gain(uint8_t *gstr, double gain);
void encode_gain(uint8_t *gainString, double gain);

bool getByteParity(uint8_t byte_);

void getParity(uint8_t *parityString, uint8_t *dataString);