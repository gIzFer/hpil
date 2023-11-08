#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

/*
For whatever reason (I think both) INT0_vect & INT1_vect trigger after a signal, luckily it does not seem to matter as of now. When sticking the scope on the pins I didn't see anything. Will wait if it gets magically fixed after redoing the analog input stage
*/


#define PULSES_ARRAY_SIZE 55

#define FRAME_CONTROL_BITS 4
#define FRAME_DATA_BITS 8
#define FRAME_SIZE (FRAME_CONTROL_BITS + FRAME_DATA_BITS)



void decodeFrame();
