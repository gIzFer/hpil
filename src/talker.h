#pragma once
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
//#include <util/delay_basic.h>
#include <stdio.h>
#include "uart.h"
#include "messages.h"

#define TALK_PIN_1 PORTD4
#define TALK_PIN_0 PORTD5
#define H_BRIDGE
//#define RS485

void setupTalker();

void talkBitOne();
void talkBitZero();


void talkOne();
void talkZero();
void talkOne_S();
void talkZero_S();

void talk(struct command commandToSend);
