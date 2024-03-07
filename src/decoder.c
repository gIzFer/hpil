#include "decoder.h"
#include <avr/interrupt.h>

//#define DEBUG_RAW_PULSES
//#define DEBUG_FRAME_CONTROL
//#define DEBUG_FRAME_DATA
//#define DEBUG_FRAME_PAIR
//#define DEBUG_TEMP_PULSE_QUEUE
#define PULSES_ARRAY_SIZE 100

volatile uint8_t pulses[PULSES_ARRAY_SIZE];
//volatile uint8_t firstBitInterrupt; // signal received, queue decoding


uint8_t dataAvailable;
uint8_t frameControl;
uint8_t frameData;



/*
#PULSES_ARRAY_SIZE
for n in range(200):
    print("r({})".format(n), end="")
*/


//24 pulses, 3 samples per pulse: 72
//duration is ~50us, 50/72
#define r(n) pulses[n] = PIND;__builtin_avr_delay_cycles(4);

//for loop is too slow D:
#define SAMPLE_SIGNAL r(0)r(1)r(2)r(3)r(4)r(5)r(6)r(7)r(8)r(9)r(10)r(11)r(12)r(13)r(14)r(15)r(16)r(17)r(18)r(19)r(20)r(21)r(22)r(23)r(24)r(25)r(26)r(27)r(28)r(29)r(30)r(31)r(32)r(33)r(34)r(35)r(36)r(37)r(38)r(39)r(40)r(41)r(42)r(43)r(44)r(45)r(46)r(47)r(48)r(49)r(50)r(51)r(52)r(53)r(54)r(55)r(56)r(57)r(58)r(59)r(60)r(61)r(62)r(63)r(64)r(65)r(66)r(67)r(68)r(69)r(70)r(71)r(72)r(73)r(74)r(75)r(76)r(77)r(78)r(79)r(80)r(81)r(82)r(83)r(84)r(85)r(86)r(87)r(88)r(89)r(90)r(91)r(92)r(93)r(94)r(95)r(96)r(97)r(98)r(99)//r(100)r(101)r(102)r(103)r(104)r(105)r(106)r(107)r(108)r(109)r(110)r(111)r(112)r(113)r(114)r(115)r(116)r(117)r(118)r(119)r(120)r(121)r(122)r(123)r(124)r(125)r(126)r(127)r(128)r(129)r(130)r(131)r(132)r(133)r(134)r(135)r(136)r(137)r(138)r(139)r(140)r(141)r(142)r(143)r(144)r(145)r(146)r(147)r(148)r(149)r(150)r(151)r(152)r(153)r(154)r(155)r(156)r(157)r(158)r(159)r(160)r(161)r(162)r(163)r(164)r(165)r(166)r(167)r(168)r(169)r(170)r(171)r(172)r(173)r(174)r(175)r(176)r(177)r(178)r(179)r(180)r(181)r(182)r(183)r(184)r(185)r(186)r(187)r(188)r(189)r(190)r(191)r(192)r(193)r(194)r(195)r(196)r(197)r(198)r(199)



void sample(){
	cli();
	PINB |= 0b00010000;
	SAMPLE_SIGNAL
	//for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
	//	pulses[i] = PIND;
	//}
	PINB |= 0b00010000;

	//firstBitInterrupt = 2;
	//EIMSK &= 0b00; //disable interrupts
	sei();
}



void addBitToFrame(uint8_t framePair_[], uint8_t position){

	uint8_t procesedBit = 0;

	if(framePair_[0] == 0 && framePair_[1] == 1){
		procesedBit = 0;
	}else if(framePair_[0] == 1 && framePair_[1] == 0){
		procesedBit = 1;
	}
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
	for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
		if((pulses[i] & 0b00001100) != 0){
			emptyCheck = 1;
			//sendByte('c');
			break;
		}
	}
	if(emptyCheck != 0){
		frameControl = 0;
		frameData = 0;




		uint8_t prevPulse = 255;
		#define STARTING_FRAME_POS 1
		uint8_t framePos = STARTING_FRAME_POS;
		#define TEMP_PULSE_QUEUE_SIZE 4
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




		for(int i = 0; i < PULSES_ARRAY_SIZE; i++){
			uint8_t pulse = pulses[i] & 0b00001100;

			//pulse translating;
			if(pulse == 8){
				pulse = 1;
			}else if(pulse == 4){
				pulse = 0;
			}else{
				pulse = 3;
			}


			//every time bit changes store it in the temp queue
			//if pulse changes (gets rid of duplication but counts how many idles in the else)
			//only dedupes 2 consecutive, random idles (noise) in between bits no
			if(pulse != prevPulse || i == (PULSES_ARRAY_SIZE - 1)){

				//if it's a bit
				if(pulse == 0 || pulse == 1 || i == (PULSES_ARRAY_SIZE - 1)){



					//if prev pulse was idle, now its a bit and count is bigger than 3 it means we're now onto the next frame bit
					//check if idle and analyze the bit queue. Commit the bit to the frame
					if(prevPulse == 3 && idleCount > 2 || i == (PULSES_ARRAY_SIZE - 1)){
						//prepare var
						framePair_ndx = 0;
						memset(framePair, 255, FRAME_PAIR_SIZE);

						#ifdef DEBUG_TEMP_PULSE_QUEUE
						for(int j = 0; j < TEMP_PULSE_QUEUE_SIZE; j++){
							if(tempPulseQueue[j] != 255){//if it has value
								sendByte(tempPulseQueue[j]+48);
							}
						}
						#endif

						//if framePos is still the default STARTING_FRAME_POS it means we're onto the first bit which is a special case
						// there are 2 bits that are not separated by an idle so first time here we read right to left instead
						if(framePos == STARTING_FRAME_POS){

							uint8_t filledValuesCount = 0;
							for(int j = TEMP_PULSE_QUEUE_SIZE - 1; j >= 0; j--){
								if(tempPulseQueue[j] != 255){//if it has value
									filledValuesCount++;
								}
							}
							framePair[0] = tempPulseQueue[filledValuesCount - 2];
							framePair[1] = tempPulseQueue[filledValuesCount - 1];
						}else{//if not first frame

							framePair[0] = tempPulseQueue[0];
							framePair[1] = tempPulseQueue[1];
						}

						//at this point we should have the right framePair
						addBitToFrame(framePair, framePos);


						#ifdef DEBUG_FRAME_PAIR
						sendByte('[');
						sendByte(framePair[0]+48);
						sendByte(framePair[1]+48);
						sendByte(']');
						sendByte('\n');
						#endif
						//clear and count on stuff
						idleCount = 0;
						framePos++;
						tempPulseQueue_ndx = 0;
						memset(tempPulseQueue, 255, TEMP_PULSE_QUEUE_SIZE);
					}


					//sendByte('>');
					//sendByte(pulse+48);
					//sendByte('\n');


					//only add if new value, sometimes a random 3 can make something like 030
					uint8_t tempPulseQueue_ndx_prev = tempPulseQueue_ndx == 0 ? (TEMP_PULSE_QUEUE_SIZE - 1) : (tempPulseQueue_ndx - 1);


					if(tempPulseQueue[tempPulseQueue_ndx_prev] != pulse){
						tempPulseQueue[tempPulseQueue_ndx] = pulse;
						tempPulseQueue_ndx++;
					}

					if(tempPulseQueue_ndx > (TEMP_PULSE_QUEUE_SIZE - 1)){
						tempPulseQueue_ndx = 0;
						//sendStr("tempPulseQueue_ndx > TEMP_PULSE_QUEUE_SIZE . should not happen");
					}

				}

				prevPulse = pulse;
			}else if(pulse == 3){
				//if its an idle count how many continuous. It separates frame bits and should be at least 3 pulses long
				idleCount++;
			}//if else

		}


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

		//sendByte('}');
		//sendByte('\n');

		dataAvailable = 1;
	}//emptyCheck




	sei();
	PINB |= 0b00010000;
}
