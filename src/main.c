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


#define LED PORTB4 //used for debugging



int main(void) {
	setupUart(F_CPU, BAUDRATE);

	setupInterrupts();
	setupTalker();
	sendStr("hi!\n\r");

	DDRB |= (1 << LED); // set pin 13  as out


	//PIND |= 0b00100000;

	for(;;) {
		talk(0b101,0b10101010);
		//decodeFrame();
		//_delay_us(1);
		//PIND |= 0b00110000;

	}
}
