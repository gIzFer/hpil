#pragma once
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
//#include <util/delay_basic.h>
#include <stdio.h>

#define TALK_PIN_1 PORTD4
#define TALK_PIN_0 PORTD5

#define PIN_1_ON_MACRO PORTD |= (0x01 << TALK_PIN_1);
#define PIN_1_OFF_MACRO PORTD &= ~(0x01 << TALK_PIN_1);
#define PIN_0_ON_MACRO PORTD |= (0x01 << TALK_PIN_0);
#define PIN_0_OFF_MACRO PORTD &= ~(0x01 << TALK_PIN_0);


void setupTalker();

void talkBitOne();
void talkBitZero();


void talkOne();
void talkZero();
void talkOne_S();
void talkZero_S();

void talk(uint8_t frameControl_, uint8_t frameData_);
