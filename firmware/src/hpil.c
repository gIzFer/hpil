#include "hpil.h"

struct command messageToSend;
static char *range[] = {
"DC_0V3 ", "DC_3V  ", "DC_30V ", "DC_300V",
"AC_V   ",
"R_300  ", "R_3k   ", "R_30k  ", "R_300k ", "R_3M   ", "R_30M  ",
"I_3    "};
struct calPair calData[RANGES_COUNT];

#include <util/delay.h>
#define F_CPU 16000000UL

//#define DEBUG_ENCODE_GAIN
//#define DEBUG_PARITY


void quickCall(uint8_t a, uint8_t b){
	messageToSend = messages[a];
	messageToSend.frameData |= b;
	PINB |= 0b00010000;
	talk(messageToSend);
	PINB |= 0b00010000;
	while(!(PIND & 0b1100)){};
	sample();
	decodeFrame(false);
}

void hpil_handle(){

	//commandParsed is set from uart
	if(uart_commandParsed == false){
		uart_commandParsed = true;

		if(uart_command[0] == 101){//custom command code 101 is reply last message receive
			messageToSend.frameControl = frameControl;
			messageToSend.frameData = frameData;
			talk(messageToSend);
		}else if(uart_command[0] == 102){//get cal data
			uint8_t inData[256];
			memset(inData, 0, 256);
			quickCall(32,0);
			quickCall(16,0);
			quickCall(32,0);
			quickCall(17,0);
			quickCall(32,0);
			quickCall(21,22);
			quickCall(32,0);
			quickCall(0,66);
			//quickCall(32,0);
			quickCall(1,50);
			quickCall(32,0);
			quickCall(23,0);
			quickCall(32,0);
			quickCall(24,22);
			quickCall(32,0);
			quickCall(36,0);
			inData[0] = frameData;
			for (int i = 1; i < 256; i++) {
				quickCall(frameControl, frameData);
				inData[i]=frameData;
			}
			quickCall(32,0);
			quickCall(27,0);
			quickCall(32,0);
			quickCall(18,0);

			//the following code is adapted from https://www.hpmuseum.org/forum/thread-8061-page-2.html
			//archive: https://web.archive.org/web/20240920172540/https://hpmuseum.org/forum/thread-8061-page-2.html
			sendStr("RAW CAL STRING: ");
			for (int i = 0; i < 256; i++) {
				sendByte(inData[i]);
			}
			sendByte('\n');
			sendStr("RRRRRRR: YYYYYYY GGGGG SSSS  ->   YYYYYYY GGGGGGGG\n");
			uint8_t currentRange = 0;
			for (int i = 16; i < (256-(16*3)); i++) {
				if (i % 16 == 0){
					currentRange = ((i+1)/16)-1;
					sendStr(range[currentRange]);
					sendByte(':');
					sendByte(' ');
				}


				inData[i] -= 64;
				sendByte(getHex(inData[i]));
				if (i % 16 == 6) sendByte(' ');
				if (i % 16 == 11) sendByte(' ');

				if ((i % 16 == 15)){
					signed char o = 0;
					if ((inData[i-15]) > 3) o  = 9;


					sendStr("  ->  ");
					sendByte(o ? '-' : ' ');
					for (uint8_t j=0; j<7; j++){
						uint8_t num = o ? o-(inData[i-15 + j]) : inData[i-15 + j];
						calData[currentRange].offset += num * (pow(10, 7-j));

						sendByte(getHex(num));
					}

					calData[currentRange].gain = getgain(inData + i - 15 + 7);
					char output[10];
					//sprintf(output, "%ld", calData[currentRange].offset);
					//sendStr(output);
					sendByte(' ');
					sprintf(output, "%1.6f", calData[currentRange].gain);
					sendStr(output);

					#ifdef DEBUG_ENCODE_GAIN
						uint8_t gainStr[5];
						encode_gain(gainStr, calData[currentRange].gain);

						sendByte(' ');
						for (uint8_t j=0; j<5; j++) {
							sendByte(getHex(gainStr[j]));
						}
					#endif

					#ifdef DEBUG_PARITY
						uint8_t parityString[4];
						getParity(parityString, inData + i - 15);

						sendByte(' ');
						sendByte(getHex(parityString[0]));
						sendByte(getHex(parityString[1]));
						sendByte(getHex(parityString[2]));
						sendByte(getHex(parityString[3]));
					#endif

					sendByte('\n');
				}
			}
		}else if(uart_command[0] == 103){//get version
			sendStr(VERSION);
			sendByte('\n');
		}else if(uart_command[0] == 104){//set cal
			//there are 24 values. 12 ranges and 2 per range.
		}else{
			messageToSend = messages[(uint8_t) uart_command[0]];
			messageToSend.frameData |= uart_command[1] & messages[(uint8_t) uart_command[0]].paramBits;
			talk(messageToSend);
		}
	}
}
