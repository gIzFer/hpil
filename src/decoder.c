#include "decoder.h"
#include <avr/interrupt.h>

volatile uint8_t pulses[PULSES_ARRAY_SIZE];

volatile uint8_t firstBitInterrupt; // signal received, queue decoding

uint8_t frameControl;
uint8_t frameData;


//starts by 1
ISR(INT0_vect){
	EIMSK &= 0b00;
	PINB |= 0b00010000;
	firstBitInterrupt = 1;
	for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
		pulses[i] = PIND;
	}
	PINB |= 0b00010000;

}

//starts by 0
ISR(INT1_vect){
	EIMSK &= 0b00;
	PINB |= 0b00010000;
	firstBitInterrupt = 2;
	for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
		pulses[i] = PIND;
	}
	PINB |= 0b00010000;
}


void decodeFrame(){
	if(firstBitInterrupt != 0){
		PINB |= 0b00010000;
		cli();

		//check if empty to save time
		uint8_t emptyCheck = 0;
		for (size_t i = 0; i < 5; i++) {//unlikely any message starts with 5 zeroes ?
			emptyCheck += pulses[i] & 0b00001100;
		}

		if(emptyCheck != 0){ // message is not empty, must start by 1


			//track where I am in the frame decoding progress, 1-12 (0-11)
			uint8_t frameDecodingPos = 1;
			//already have the first bit from which ISR triggered
			frameControl = (firstBitInterrupt & 0b10) << 2;
			//control = 0;
			frameData = 0;


			uint8_t dataPair_[2];
			memset(dataPair_, 0, 2);
			uint8_t dataPair_ndx_ = 0;
			uint8_t dataPair_val_prevPair = 0;


			for(int i = 0; i < PULSES_ARRAY_SIZE; i++){
				uint8_t pulse = pulses[i] & 0b00001100;

				//pulse translating;

				if(pulse == 8){//8 means 0
					pulse = 0;
				}else if(pulse == 4){//4 means 1
					pulse = 1;
				}else if(pulse == 12){//12 shouldn't happen, signals overlapped, should be treated like an idle I guess
					pulse = 2;
				}else{//0 idle
					pulse = 3;
				}


				if(pulse == 1 || pulse == 0){//ignore idles
					//first val and its not the same as the inmediately previous one, it needs a 3 between to equal ones to be valid
					//abusing order of check so it doesnt evaluate pulses[i - 1] on the first run as that would crash
					if(dataPair_ndx_ == 0 && pulse != dataPair_val_prevPair){
						dataPair_[0] = pulse;
						dataPair_ndx_++;
					}else if(dataPair_ndx_ == 1 && dataPair_[0] != pulse){ //not repeated, for deduplication of oversampling
						dataPair_val_prevPair = pulse;
						dataPair_[1] = pulse;
						dataPair_ndx_ = 0;


						//here I must have a pair of 0 and 1
						if(frameDecodingPos < FRAME_SIZE){
							//get the value of the bit pair
							uint8_t procesedBit = 0;
							if(dataPair_[0] == 0 && dataPair_[1] == 1){
								procesedBit = 1;
							}else if(dataPair_[0] == 1 && dataPair_[1] == 0){
								procesedBit = 0;
							}else{//no?
								frameControl = 255;
							}




							//add it to the result
							//frame decoding pos is in array, the constant is a total value, gotta -1 it
							if(frameDecodingPos < (FRAME_CONTROL_BITS)){
								frameControl |= procesedBit << (FRAME_CONTROL_BITS - frameDecodingPos - 1);
								frameDecodingPos++;
							}else{//its data
								frameData |= procesedBit << (FRAME_CONTROL_BITS + FRAME_DATA_BITS - frameDecodingPos - 1);
								frameDecodingPos++;
							}


						}else{
							frameControl = 255;
						}
					}
				}else{ //pulse was idle, so a new pulse can start with the same digit as the prev one. Set the previous value as idle and so we can start a new pair in the next loop
					dataPair_val_prevPair = 3;
				}
			}//end of for
		}//end of empty check

		//enable interrupts
		firstBitInterrupt = 0;
		EIMSK |= 0b11; //enable INT0 and INT1
		sei();
		PINB |= 0b00010000;


	//char message_[32];
	//sprintf(message_, "%d - %d\r\n", control, data);
	//sendStr(message_);


	//sendByte(control);
	sendByte(frameData);

	//sendByte('\n');

	}
}
