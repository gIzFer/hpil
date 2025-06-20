#include "hpil.h"

struct command messageToSend;

#include <util/delay.h>
#define F_CPU 16000000UL




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

char getHex(uint8_t num){
	if(num < 10){
		return num + 48;
	}else{
		return num + 55;
	}
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

			//the following code is adapted from https://www.hpmuseum.org/forum/thread-8061-page-2.html
			//archive: https://web.archive.org/web/20240920172540/https://hpmuseum.org/forum/thread-8061-page-2.html
			static char *range[] = {
			"",
			"0.3V", "3V  ", "30V ", "300V",
			"ACV ",
			"300R", "3k  ", "30k ", "300k", "3M  ", "30M ",
			"3A  ",
			"    ", "    ", "    " };
			sendStr("RRRR: YYYYYYY GGGGG SSSS  YYYYYYY GGGGG\n      ");
			for (int i = 0; i < 256; i++) {
				inData[i] -= 64;
				sendByte(getHex(inData[i]));

				if (i % 16 == 6) sendByte(' ');
				if (i % 16 == 11) sendByte(' ');

				if ((i % 16 == 15)){
					uint8_t o = 0;
					sendByte(' ');
					if ((inData[i-15]) > 3) o = 9;
					sendByte(o ? '-' : ' ');
					for (uint8_t j=0; j<7; j++){
						sendByte((o ? getHex(o-(inData[i-15 + j])) : getHex(inData[i-15 + j]) ));
					}
					sendByte(' ');
					float f = 1.0;
					float s = 100.0;
					for (uint8_t j=7; j<12; j++) {
						o = inData[i-15 + j];
						o = o > 7 ? o - 16 : o;
						f += ((float) o ) / s;
						s *= 10;
					}
					char output[12];
					sprintf(output, "%1.8f", f);
					sendStr(output);
					sendByte('\n');
					if(i != 255){
						sendStr(range[(i+1)/16]);
						sendByte(':');
						sendByte(' ');
					}
				}
			}
		}else{
			messageToSend = messages[(uint8_t) uart_command[0]];
			messageToSend.frameData |= uart_command[1] & messages[(uint8_t) uart_command[0]].paramBits;
			talk(messageToSend);
		}
	}
}
