#include "interrupts.h"
void setupInterrupts(){
	SREG |= 1<<SREG_I; //global interurpt enable
}
