//configs
#define F_CPU 16000000UL

#define BAUDRATE 115200

#define VERBOSE
#define DEBUG_PIN


#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>


#include "interrupts.h"
#include "uart.h"
#include "decoder.h"
#include "talker.h"
#include "hpil.h"
#include "version.h"

#ifdef DEBUG_PIN
	#define LED PORTB4 //pin 12 for debugging
#endif

int main(void) {
#ifdef DEBUG_PIN
	DDRB |= (1 << LED); // set pin 12 as out
#endif
	setupUart(F_CPU, BAUDRATE);

	setupTalker();

	setupInterrupts();

	sendStr(VERSION);
	sendStr("\nready\n");
	for(;;) {
		if((PIND & 0b1100)){//if any of the input pins read a high
			sample();
			decodeFrame(true);
		}
		if(uart_commandParsed == false) //needed as the time to go into the function is too much
		hpil_handle();
	}
}
