#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "uart.h"

/*
For whatever reason (I think both) INT0_vect & INT1_vect trigger after a signal, luckily it does not seem to matter as of now. When sticking the scope on the pins I didn't see anything. Will wait if it gets magically fixed after redoing the analog input stage
*/



#define FRAME_CONTROL_BITS 3
#define FRAME_DATA_BITS 8
#define FRAME_SIZE (FRAME_CONTROL_BITS + FRAME_DATA_BITS)

//extern volatile uint8_t pulses[PULSES_ARRAY_SIZE];

//extern volatile uint8_t firstBitInterrupt; // signal received, queue decoding

extern uint8_t dataAvailable;
extern uint8_t frameControl;
extern uint8_t frameData;

void sample();
void addBitToFrame(uint8_t framePair_[], uint8_t position);
void decodeFrame();
