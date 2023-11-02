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





int main(void) {
	setupUart(F_CPU, BAUDRATE);

	setupInterrupts();
	//sendStr("hi!\n\r");


	for(;;) {
		decodeFrame();


	}
}
