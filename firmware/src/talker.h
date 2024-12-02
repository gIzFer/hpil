#pragma once
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
//#include <util/delay_basic.h>
#include <stdio.h>
#include "uart.h"
#include "messages.h"

//IMPORTANT: PINS NEED TO BE CONSECUTIVE AND FROM THE SAME PORT
//HERE PINS 4 AND 5 ARE USED BY DEFAULT
#define TALK_PIN_PORT PORTD

//THE PIN WITH THE SMALLER NUMBER (HERE 4) HAS TO BE PIN 1
#define TALK_PIN_1 PORTD4
#define TALK_PIN_0 PORTD5


void setupTalker();

void talkBitOne();
void talkBitZero();


void talkOne();
void talkZero();
void talkOne_S();
void talkZero_S();

void talk(struct command commandToSend);
