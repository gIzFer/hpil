#include "decoder.h"
#include <avr/interrupt.h>

//cursed optimization
#define STRINGIFY(X) #X
#define OPTIMIZE_LOOP_PULSES_ARRAY_SIZE(X) _Pragma(STRINGIFY(X))
#define OPTIMIZE_PARAMS GCC unroll

//#define DEBUG_FUNC_addBitToFrame

//#define DEBUG_RAW_PULSES
//#define DEBUG_DEDUPLICATED_PULSES
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
//message duration is ~50us, 50/72
//each reading cycle takes 500ns

void sample(){
	cli();
	PINB |= 0b00010000;
	OPTIMIZE_LOOP_PULSES_ARRAY_SIZE(OPTIMIZE_PARAMS PULSES_ARRAY_SIZE)
	for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
		pulses[i] = PIND;
		__builtin_avr_delay_cycles(4);
	}

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

	OPTIMIZE_LOOP_PULSES_ARRAY_SIZE(OPTIMIZE_PARAMS PULSES_ARRAY_SIZE)
	for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
		if((pulses[i] & 0b00001100) != 0){
			emptyCheck = 1;
			break;
		}
	}
	if(emptyCheck != 0){
		frameControl = 0;
		frameData = 0;




		uint8_t prevPulse = 255;
		#define STARTING_FRAME_POS FRAME_SIZE
		uint8_t framePos = STARTING_FRAME_POS - 1;
		#define TEMP_PULSE_QUEUE_SIZE 36
		uint8_t tempPulseQueue[TEMP_PULSE_QUEUE_SIZE];
		memset(tempPulseQueue, 255, TEMP_PULSE_QUEUE_SIZE);
		uint8_t tempPulseQueue_ndx = 0;








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
		//#define unrollLoop _Pragma(GCC unroll PULSES_ARRAY_SIZE)
		OPTIMIZE_LOOP_PULSES_ARRAY_SIZE(OPTIMIZE_PARAMS PULSES_ARRAY_SIZE)
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

		#ifdef DEBUG_DEDUPLICATED_PULSES/////////////////////////////////
		for(int i = 0; i < TEMP_PULSE_QUEUE_SIZE; i++){
			uint8_t pulse = tempPulseQueue[i];
			//sendByte(pulse+48);
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

		//parse tempPulseQueue into frameControl and frameData
		OPTIMIZE_LOOP_PULSES_ARRAY_SIZE(OPTIMIZE_PARAMS TEMP_PULSE_QUEUE_SIZE)
		for(int i = tempPulseQueue_ndx; i > 0 ; i--){
			if(tempPulseQueue[i] == 8 || tempPulseQueue[i] == 4){
				addBitToFrame(tempPulseQueue + i - 1, framePos);
				framePos--;
				i--;
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
