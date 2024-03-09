#include "decoder.h"
#include <avr/interrupt.h>

//#define DEBUG_FUNC_addBitToFrame

//#define DEBUG_RAW_PULSES
//#define DEBUG_FRAME_CONTROL
//#define DEBUG_FRAME_DATA
//#define DEBUG_FRAME_ENABLE_NEW_LINE

#define PULSES_ARRAY_SIZE 100

volatile uint8_t pulses[PULSES_ARRAY_SIZE];


#define PULSE_1 8
#define PULSE_0 4

uint8_t dataAvailable;
uint8_t frameControl;
uint8_t frameData;






//24 pulses, 3 samples per pulse: 72
//duration is ~50us, 50/72
#define r(n) pulses[n] = PIND;__builtin_avr_delay_cycles(4);

//for loop is too slow D:
/*
generate with python:
for n in range(200):
    print("r({})".format(n), end="")
*/
#define SAMPLE_SIGNAL r(0)r(1)r(2)r(3)r(4)r(5)r(6)r(7)r(8)r(9)r(10)r(11)r(12)r(13)r(14)r(15)r(16)r(17)r(18)r(19)r(20)r(21)r(22)r(23)r(24)r(25)r(26)r(27)r(28)r(29)r(30)r(31)r(32)r(33)r(34)r(35)r(36)r(37)r(38)r(39)r(40)r(41)r(42)r(43)r(44)r(45)r(46)r(47)r(48)r(49)r(50)r(51)r(52)r(53)r(54)r(55)r(56)r(57)r(58)r(59)r(60)r(61)r(62)r(63)r(64)r(65)r(66)r(67)r(68)r(69)r(70)r(71)r(72)r(73)r(74)r(75)r(76)r(77)r(78)r(79)r(80)r(81)r(82)r(83)r(84)r(85)r(86)r(87)r(88)r(89)r(90)r(91)r(92)r(93)r(94)r(95)r(96)r(97)r(98)r(99)



void sample(){
	cli();
	PINB |= 0b00010000;
	SAMPLE_SIGNAL
	PINB |= 0b00010000;
	sei();
}



void addBitToFrame(uint8_t framePair_[], uint8_t position){

	uint8_t procesedBit = 0;

	if(framePair_[0] == PULSE_0 && framePair_[1] == PULSE_1){
		procesedBit = 0;
	}else if(framePair_[0] == PULSE_1 && framePair_[1] == PULSE_0){
		procesedBit = 1;
	}

	#ifdef DEBUG_FUNC_addBitToFrame
	sendByte('{');
	sendByte(position + 48);
	sendByte(':');
	sendByte(procesedBit + 48);
	sendByte('}');
	#endif
	if(position < (FRAME_CONTROL_BITS)){
		//the bit is stored from MSB to LSB / left to right: 3 - pos(1,2,3)
		frameControl |= procesedBit << (FRAME_CONTROL_BITS - 1 - position);
	}else{//if position is _equal or bigger_ than 4 (FRAME_CONTROL_BITS) its data
		//frameDecodingPos goes up to 11. we need it to go only up to 7 for the byte
		//remove the FRAME_CONTROL_BITS from the post
		uint8_t dataPosition = position - FRAME_CONTROL_BITS;
		frameData |= procesedBit << (FRAME_DATA_BITS - 1 - dataPosition);
	}
}


void decodeFrame(){
	PINB |= 0b00010000;
	cli();

	//check if empty to save time
	uint8_t emptyCheck = 0;
	for (uint8_t i = 0; i < PULSES_ARRAY_SIZE / 3; i++) {
		if((pulses[i] & 0b00001100) != 0){
			emptyCheck = 1;
			break;
		}
	}
	if(emptyCheck != 0){
		frameControl = 0;
		frameData = 0;




		uint8_t prevPulse = 255;
		#define STARTING_FRAME_POS 1
		uint8_t framePos = STARTING_FRAME_POS;
		#define TEMP_PULSE_QUEUE_SIZE 40
		uint8_t tempPulseQueue[TEMP_PULSE_QUEUE_SIZE];
		memset(tempPulseQueue, 255, TEMP_PULSE_QUEUE_SIZE);
		uint8_t tempPulseQueue_ndx = 0;

		uint8_t idleCount = 0;

		#define FRAME_PAIR_SIZE 2
		uint8_t framePair[2];
		memset(framePair, 255, FRAME_PAIR_SIZE);
		uint8_t framePair_ndx = 0;






		#ifdef DEBUG_RAW_PULSES/////////////////////////////////
		for(int i = 0; i < PULSES_ARRAY_SIZE; i++){
			uint8_t pulse = pulses[i] & 0b00001100;

			//pulse translating;

			if(pulse == 8){//8 means 0
				pulse = 1;
			}else if(pulse == 4){//4 means 1
				pulse = 0;
			}else{//0 idle
				pulse = 3;
			}

			sendByte(pulse+48);
		}
		sendByte('\n');
		#endif//////////////////////////////////



		//deduplicate and create simple tempPulseQueue
		for(int i = 1; i < PULSES_ARRAY_SIZE; i++){
			uint8_t pulse = pulses[i] & 0b00001100;
			uint8_t pulse_prev = ((i-1) < PULSES_ARRAY_SIZE) ? (pulses[i-1] & 0b00001100) : 255;
			uint8_t pulse_next = ((i+1) < PULSES_ARRAY_SIZE) ? (pulses[i+1] & 0b00001100) : 255;

			if(pulse != prevPulse){//if pulse is different than the last stored one
				//exception: random idles make it through, but its no more than 1 single one, on which case we skip
				//if pulse is 0 but neither nexty or prev pulses are skip this run
				if(pulse == PULSE_1 || pulse == PULSE_0 || (pulse == 0 && (pulse_prev == 0 || pulse_next == 0))) {
					tempPulseQueue[tempPulseQueue_ndx] = pulse;
					tempPulseQueue_ndx++;
				}
				prevPulse = pulse;
			}
		}

		//parse tempPulseQueue into frameControl and frameData
		for(int i = 2; i < tempPulseQueue_ndx; i++){
			if(tempPulseQueue[i] == 0){
				addBitToFrame(tempPulseQueue + i -2, framePos);
				framePos++;
			}
		}

		//sendByte('r');
		sendByte(frameControl);
		sendByte(frameData);
		sendByte('\n');


		PINB |= 0b00010000;
		PINB |= 0b00010000;

		#ifdef DEBUG_FRAME_CONTROL
		sendByte(frameControl);
		#endif

		#ifdef DEBUG_FRAME_DATA
		sendByte(frameData);
		#endif

		#ifdef DEBUG_FRAME_ENABLE_NEW_LINE
		sendByte('\n');
		#endif

		dataAvailable = 1;
	}//emptyCheck


	sei();
	PINB |= 0b00010000;
}
