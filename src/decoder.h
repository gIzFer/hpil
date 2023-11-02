#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"



#define PULSES_ARRAY_SIZE 55

#define FRAME_CONTROL_BITS 4
#define FRAME_DATA_BITS 8
#define FRAME_SIZE (FRAME_CONTROL_BITS + FRAME_DATA_BITS)



void decodeFrame();
