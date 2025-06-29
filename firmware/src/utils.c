#include "utils.h"

char getHex(uint8_t num){
	if(num < 10){
		return num + 48;
	}else{
		return num + 55;
	}
}

//from https://github.com/fenugrec/hp3478a_utils/blob/master/hp3478util.c
double getgain(const uint8_t *gainString) {
	double gain = 1.0;
	double mult = 0.01;
	unsigned cur;
	uint8_t dig;

	for (cur = 0; cur <= 4; cur += 1) {
		dig = gainString[cur] & 0x0F;
		if (dig & 0x08) {
			// digit is negative, i.e. 0x0C is -4
			gain += (dig-16) * mult;
		} else {
			gain += dig * mult;
		}
		mult = mult / 10;
	}
	return gain;
}

uint8_t getNDigit(double gain, uint8_t digitNumber){
	long kf_int = round(gain*1.0E6);
	if(gain>1){
		kf_int -= 1E6;
	}
	return (long)kf_int/(long)(pow(10,digitNumber-1))%10 ;
}


// based on https://www.eevblog.com/forum/repair/hp-3478a-how-to-readwrite-cal-sram/msg1966463/#msg1966463

//gain is read right to left
//if the first character is greater than 6 we add 6 to it and add one to the next value (addOneToNext=1). If not digit is set as is

//on all the characters after:
//first we add 1 from the previous character if needed
//then, if its 10 we set the digit as 0 and add one to the next value
//if the digit (after adding one) is more than 4 we add 6 and add one to the next value. If not digit is set as is
void encode_gain(uint8_t *gainString, double gain){
	uint8_t addOneToNext = 0;
	for (int j=1; j<6; j++) {
		uint8_t lastDigit = getNDigit(gain,j);
		//printf("%d",lastDigit);
		lastDigit+=addOneToNext;
		addOneToNext = 0;
		if(lastDigit == 10){
			lastDigit=0;
			addOneToNext = 1;
		}
		if(j == 1){
			if(lastDigit > 6){
				lastDigit+=6;
				addOneToNext = 1;
			}
		}else{
			if(lastDigit > 4){
				lastDigit+=6;
				addOneToNext = 1;
			}
		}
		gainString[5-j]=lastDigit;
	}
}

bool getByteParity(uint8_t byte_){
	bool parity = false;
	while (byte_){
		if (byte_ & 0b1){
			parity = !parity;
		}
		byte_ >>= 1;
	}
	return parity;
}

//https://www.hpmuseum.org/forum/thread-8061-post-190013.html#pid190013

/*
                            │ E: d 1101 │ F: 7 01░░ │ 1
                                  ²↑↑↑↑       ²↑↑     │
│ 0: 0 0000 │ 6: 6 0110 │ 1 ┈┈┈┈┈┈┈┘│││        ││     │
│ 1: 0 0000 │ 7: f 1111 │ 1 ┈┈┈┈┈┈┈┈┘││        ││     │
│ 2: 0 0000 │ 8: 4 0100 │ 0 ┈┈┈┈┈┈┈┈┈┘│        ││     │
│ 3: 0 0000 │ 9: 3 0011 │ 1 ┈┈┈┈┈┈┈┈┈┈┘        ││     │
│ 4: 0 0000 │ A: d 1101 │ 0 ┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┘│     │
│ 5: 0 0000 │ B: c 1100 │ 1 ┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┘     │
      ¹↓↓↓↓       ¹↓↓↓↓                               │
│ C: f 1111 │ D: 0 0000 │ 1 ┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┐┌┈┈┈┘
                                                ³↓↓⁴
                            │ E: d 1101 │ F: 7 0111 │
*/
void getParity(uint8_t *parityString, uint8_t *dataString){

	//odd parity of first column (nibble C)
	parityString[0] = (~(dataString[0]^dataString[1]^dataString[2]^dataString[3]^dataString[4]^dataString[5]))&0xF;

	//odd parity of second column (nibble D)
	parityString[1] = (~(dataString[6]^dataString[7]^dataString[8]^dataString[9]^dataString[10]^dataString[11]))&0xF;

	//get the rows parity into a byte
	uint8_t rowsParity = 0;
	for(uint8_t x = 0; x < 6; x++){
		uint8_t row = (dataString[x]<<4)|dataString[x+6];
		bool myParity = !getByteParity(row);
		rowsParity <<=1;
		rowsParity |= myParity;
	}

	//fill parity nibble E
	parityString[2] = rowsParity >> 2;
	//fill parity nibble F
	parityString[3] = (rowsParity & 0b11) << 2;
	bool C_D_parity = !getByteParity((parityString[0]<<4)|parityString[1]);
	bool E_F_parity = !getByteParity((parityString[2]<<4)|parityString[3]);
	parityString[3] |= C_D_parity << 1 | E_F_parity;
}


