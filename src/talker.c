#include "talker.h"

void setupTalker(){
	DDRD |= (1 << TALK_PIN_1);
	DDRD |= (1 << TALK_PIN_0);
}

void talk(uint8_t frameControl_, uint8_t frameData_){
	cli();

	#define TIME_BETWEEN_CONTROL_PULSES 18
	if(frameControl_ & 0b100){
		talkOne_S();
	}else{
		talkZero_S();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_CONTROL_PULSES);

	if(frameControl_ & 0b10){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_CONTROL_PULSES);

	if(frameControl_ & 0b1){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_CONTROL_PULSES);








	//DATA------------------------------------------------------------------------
	#define TIME_BETWEEN_DATA_PULSES 18
	if(frameControl_ & 0b1 << 7){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(frameControl_ & 0b1 << 6){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(frameControl_ & 0b1 << 5){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(frameControl_ & 0b1 << 4){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(frameControl_ & 0b1 << 3){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(frameControl_ & 0b1 << 2){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(frameControl_ & 0b1 << 1){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(frameControl_ & 0b1 << 0){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);







	//for is too slow
	/*for (uint8_t i = 8; i >= 1; i--) {
		if(frameData_ & 0b1 << i){
			talkOne();
		}else{
			talkZero();
		}
		//__builtin_avr_delay_cycles(12);
	}*/

	//__builtin_avr_delay_cycles(16*100);
	sei();
}


#define TIME_BETWEEN_PULSES 14
void talkOne(){
	PIN_1_ON_MACRO
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = 0b10 << TALK_PIN_1; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PIN_0_OFF_MACRO
}
void talkZero(){
	PIN_0_ON_MACRO
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = 0b01 << TALK_PIN_1; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PIN_1_OFF_MACRO
}

void talkOne_S(){
	PIN_1_ON_MACRO
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = 0b10 << TALK_PIN_1; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = 0b01 << TALK_PIN_1; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = 0b10 << TALK_PIN_1; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PIN_0_OFF_MACRO
}
void talkZero_S(){
	PIN_0_ON_MACRO
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = 0b01 << TALK_PIN_1; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = 0b10 << TALK_PIN_1; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = 0b01 << TALK_PIN_1; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PIN_1_OFF_MACRO
}
