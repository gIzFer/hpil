#include "interrupts.h"
void setupInterrupts(){

	//set interrupt
	EICRA |= 0b11; //INT0 on rising edge
	EIMSK |= 0b01; //enable INT0

	EICRA |= 0b1100; //INT1 on rising edge
	EIMSK |= 0b10; //enable INT1

	//configure pin interrupts //faster to directly int0
	//PCICR |= 1 << 2; //enable PCIE0 to receive interrupt from pin 0 to 7
	//PCMSK2 |= 1 << 2; //select pins that trigger interrupt on pcicr

	SREG |= 1<<SREG_I; //global interurpt enable


}
