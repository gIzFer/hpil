#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "uart.h"
#include "decoder.h"
#include "talker.h"
#include "messages.h"
#include "version.h"
#include "utils.h"
#include <math.h>

struct sentCommandState{
	struct command commandSent;
	bool hasBeenReceivedBackFromLoop;
};

void configureTimeout(uint8_t time);
void startTimeout();
void stopTimeout();

void hpil_setup();
void hpil_handle();

void sendCmd(struct command commandToSend);
void failedCmd();

#define RANGES_COUNT 12

enum range {
	DC_0V3 = 0,
	DC_3V = 1,
	DC_30V = 2,
	DC_300V = 3,
	AC_V = 4,
	R_300 = 5,
	R_3k = 6,
	R_30k = 7,
	R_300k = 8,
	R_3M = 9,
	R_30M = 10,
	I_3 = 11

};


struct calPair {
	uint32_t offset;
	double gain;
};
