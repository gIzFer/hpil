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
	setupUart(F_CPU, BAUDRATE);

	setupTalker();
	hpil_setup();
	setupInterrupts();
	sendByte(MCUSR);
	sendByte(MCUSR);
	sendByte(MCUSR);
	sendStr("hi!\n");

	DDRB |= (1 << LED); // set pin 12 as out


	//PIND |= 0b00100000;

	for(;;) {
		if((PIND & 0b1100)){
			//__builtin_avr_delay_cycles(10);
			//PINB |= 0b00010000;
			sample();
			//PINB |= 0b00010000;
			//talk(0b101,0b10101010);
			decodeFrame();
			//talk((struct command){255, frameControl, frameData});

		}
		//talk((struct command){255, 'b', 'w'});

		hpil_handle();

		//parseCommand();

		//_delay_us(100);
		//PIND |= 0b00110000;

	}
}
