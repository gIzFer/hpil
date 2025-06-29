#include "hpil.h"

struct command messageToSend;
static char *range[] = {
"DC_0V3 ", "DC_3V  ", "DC_30V ", "DC_300V",
"AC_V   ",
"R_300  ", "R_3k   ", "R_30k  ", "R_300k ", "R_3M   ", "R_30M  ",
"I_3    "};
struct calPair calData[RANGES_COUNT];
uint8_t finalStr[256];

#include <util/delay.h>
#define F_CPU 16000000UL

//#define DEBUG_FORMAT_RAW_STRINGS

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
void generateCalString(){

	memset(finalStr, 0, 256);

	for(uint8_t currentRange = 0; currentRange < RANGES_COUNT; currentRange++){
		char buffer[8];
		int32_t offset_ = calData[currentRange].offset;
		if(offset_ < 0) offset_ = 1E7+offset_;
		sprintf(buffer, "%07ld", offset_);
		for(uint8_t offsetPos = 0;offsetPos < 7; offsetPos++){
			finalStr[16+currentRange*16+offsetPos] = buffer[offsetPos]-48;
		}
		uint8_t gainStr[5];
		encode_gain(gainStr, calData[currentRange].gain);

		for(uint8_t gainPos = 0;gainPos < 5; gainPos++){
			finalStr[16+currentRange*16+gainPos+7] = gainStr[gainPos];
		}


		uint8_t parityString[4];
		getParity(parityString, finalStr + 16+currentRange*16);
		for(uint8_t parityPos = 0; parityPos < 4; parityPos++){
			finalStr[16+currentRange*16+parityPos+12] = parityString[parityPos];
		}

	}
	for(uint16_t nibblePos = 0; nibblePos <= 255; nibblePos++){
		finalStr[nibblePos]+=64;
	}
}

void printCalData(){
	sendStr("Loaded calibration data:\n");
	sendStr("RRRRRRR:  YYYYYYY	GGGGGGGG GGGGG\n");
	for(uint8_t currentRange = 0; currentRange < RANGES_COUNT; currentRange++){
		sendStr(range[currentRange]);
		sendByte(':');
		sendByte(' ');

		char output[10];
		char offsetTemplateStr[] = " %07ld";
		if(calData[currentRange].offset < 0) memcpy(offsetTemplateStr, "%08ld\0", 6);
		sprintf(output, offsetTemplateStr, calData[currentRange].offset);
		sendStr(output);

		sendByte(' ');
		sprintf(output, "%1.6f", calData[currentRange].gain);
		sendStr(output);

		uint8_t gainStr[5];
		encode_gain(gainStr, calData[currentRange].gain);

		sendByte(' ');
		for (uint8_t j=0; j<5; j++) {
			sendByte(getHex(gainStr[j]));
		}
		sendByte('\n');
	}

	sendStr("RAW CAL STRING: ");
	for(uint8_t row = 0; row < 16; row++){
		for(uint8_t x = 0; x < 16; x++){
			sendByte(finalStr[row*16+x]);
			#ifdef DEBUG_FORMAT_RAW_STRINGS
				if (x == 6) sendByte(' ');
				if (x == 11) sendByte(' ');
			#endif
		}
		#ifdef DEBUG_FORMAT_RAW_STRINGS
			sendByte('\n');
		#endif
	}
	sendByte('\n');
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
			//so about decoding the offset its calculated subtracting from 9999999 and making it negative.
			//i.e.: 9999992 would be -7.
			//But on my cal dump I was having both 0000000 and 9999999 which with this logic would be the same.
			//As it wouldnt make sense to send 9999999 I deduced that the value is subtracted from 1E7, then the 7 zeroes would mean 0 and the 7 nines would be -1
			sendStr("RAW CAL STRING: ");
			for (int i = 0; i < 256; i++) {
				#ifdef DEBUG_FORMAT_RAW_STRINGS
					if (i % 16 == 0){
						sendByte('\n');
					}
				#endif
				sendByte(inData[i]);
				#ifdef DEBUG_FORMAT_RAW_STRINGS
					if (i % 16 == 6) sendByte(' ');
					if (i % 16 == 11) sendByte(' ');
				#endif
			}
			sendByte('\n');
			sendStr("RRRRRRR: YYYYYYY GGGGG SSSS\n");
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
					if ((inData[i-15]) > 3) o	= 9;
					for (uint8_t j=0; j<7; j++)
						calData[currentRange].offset += inData[i-15 + j] * (pow(10, 6-j));

					if(o) calData[currentRange].offset -= 1E7;
					calData[currentRange].gain = getgain(inData + i - 15 + 7);

					sendByte('\n');
				}
			}
			generateCalString();
		}else if(uart_command[0] == 103){//get version
			sendStr(VERSION);
			sendByte('\n');
		}else if(uart_command[0] == 104){//set cal
			//there are 24 values. 12 ranges and 2 per range.
			//uart_command[1] indicates which value is the data for. even numbers are offsets, odd are gain
			if(uart_command[1]>23){
				sendByte(202);
				sendByte('\n');
				return;
			}

			if(uart_command[1] % 2){//gain
				memcpy(&calData[(uart_command[1] / 2)].gain, uart_command + 2, 4);
			}else{//offset
				memcpy(&calData[(uart_command[1] / 2)].offset, uart_command + 2, 4);
			}
		}else if(uart_command[0] == 105){//print loaded cal data
			printCalData();
		}else{
			messageToSend = messages[(uint8_t) uart_command[0]];
			messageToSend.frameData |= uart_command[1] & messages[(uint8_t) uart_command[0]].paramBits;
			talk(messageToSend);
		}
	}
}
