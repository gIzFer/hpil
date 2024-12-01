//configs
#define F_CPU 16000000UL

#define BAUDRATE 115200

#define VERBOSE



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


#define LED PORTB4 //used for debugging


int main(void) {
	DDRB |= (1 << LED); // set pin 12 as out
	setupUart(F_CPU, BAUDRATE);

	setupTalker();

	setupInterrupts();



	//sendStr("hi!\n");

	for(;;) {
		if((PIND & 0b1100)){
			sample();
			decodeFrame();
		}
		hpil_handle();
	}
}
