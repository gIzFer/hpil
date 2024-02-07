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


struct commandSentStruct{
	bool frameControl;
	bool frameData;
	bool hasBeenReceivedBackFromLoop;

}commandSent = {0, 0, 0};
int main(void) {
	setupUart(F_CPU, BAUDRATE);

	setupTalker();
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
			if(dataAvailable != 0){
				//sendByte(frameData);
				dataAvailable = 0;
				//sendByte('>');
				//sendByte(frameControl);
				//sendByte(frameData);
				//sendByte('\n');
				if(commandSent.hasBeenReceivedBackFromLoop == false/* && commandSent.frameControl == frameControl && commandSent.frameData == frameData*/){
					commandSent.hasBeenReceivedBackFromLoop = true;
				}else{
					//talk(frameControl, frameData);
				}
				//_delay_us(1);
			}
		}
		//parseCommand();
		if(commandParsed == false){
			//sendStr(command);
			commandParsed = true;
			if(command[0] == 'a'){ //IFC interface clear
				uint8_t frameControl_ = 0b100;
				uint8_t frameData_ = 0b10010001;
				talk(frameControl_, frameData_);
				commandSent.hasBeenReceivedBackFromLoop = false;
				commandSent.frameControl = frameControl_;
				commandSent.frameData = frameData_;
			}

			if(command[0] == 'b'){ //RFC make sure em ready for command
				uint8_t frameControl_ = 0b101;
				uint8_t frameData_ = 0;
				talk(frameControl_, frameData_);
				commandSent.hasBeenReceivedBackFromLoop = false;
				commandSent.frameControl = frameControl_;
				commandSent.frameData = frameData_;
			}

			if(command[0] == 'c'){ //AAD auto address NO USE
				uint8_t frameControl_ = 0b101;
				uint8_t frameData_ = 0b10000000;
				talk(frameControl_, frameData_);
				commandSent.hasBeenReceivedBackFromLoop = false;
				commandSent.frameControl = frameControl_;
				commandSent.frameData = frameData_;
			}





			if(command[0] == 'd'){ //LAD
				uint8_t frameControl_ = 0b100;
				uint8_t frameData_ = 0b001 | 22;
				talk(frameControl_, frameData_);
				commandSent.hasBeenReceivedBackFromLoop = false;
				commandSent.frameControl = frameControl_;
				commandSent.frameData = frameData_;
			}

			if(command[0] == 'e'){ //
				uint8_t frameControl_ = 0b000;
				uint8_t frameData_ = 'D';
				talk(frameControl_, frameData_);
				commandSent.hasBeenReceivedBackFromLoop = false;
				commandSent.frameControl = frameControl_;
				commandSent.frameData = frameData_;
			}

			if(command[0] == 'f'){ //
				uint8_t frameControl_ = 0b000;
				uint8_t frameData_ = '2';
				talk(frameControl_, frameData_);
				commandSent.hasBeenReceivedBackFromLoop = false;
				commandSent.frameControl = frameControl_;
				commandSent.frameData = frameData_;
			}

			if(command[0] == 'g'){ //
				uint8_t frameControl_ = 0b000;
				uint8_t frameData_ = 'W';
				talk(frameControl_, frameData_);
				commandSent.hasBeenReceivedBackFromLoop = false;
				commandSent.frameControl = frameControl_;
				commandSent.frameData = frameData_;
			}

			if(command[0] == 'h'){ //
				uint8_t frameControl_ = 0b010;
				uint8_t frameData_ = 'w';
				talk(frameControl_, frameData_);
				commandSent.hasBeenReceivedBackFromLoop = false;
				commandSent.frameControl = frameControl_;
				commandSent.frameData = frameData_;
			}
		}
		//_delay_us(1);
		//PIND |= 0b00110000;

	}
}
