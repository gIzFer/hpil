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
