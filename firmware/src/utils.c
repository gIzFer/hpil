#include "utils.h"

char getHex(uint8_t num){
	if(num < 10){
		return num + 48;
	}else{
		return num + 55;
	}
}

uint8_t getNDigit(double gain, uint8_t digitNumber){
	long kf_int = round(gain*1.0E6);
	if(gain>1){
		kf_int -= 1E6;
	}
	return (long)kf_int/(long)(pow(10,digitNumber-1))%10 ;
}

// based on https://www.eevblog.com/forum/repair/hp-3478a-how-to-readwrite-cal-sram/msg1966463/#msg1966463
void encode_gain(uint8_t *gstr, double gain){
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
		gstr[5-j]=lastDigit;
	}
}

//from https://github.com/fenugrec/hp3478a_utils/blob/master/hp3478util.c
double getgain(const uint8_t *gstr) {
	double gain = 1.0;
	double mult = 0.01;
	unsigned cur;
	uint8_t dig;

	for (cur = 0; cur <= 4; cur += 1) {
		dig = gstr[cur] & 0x0F;
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
