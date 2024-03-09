#include "talker.h"

void setupTalker(){
	DDRD |= (1 << TALK_PIN_1);
	DDRD |= (1 << TALK_PIN_0);
}

void talk(struct command commandToSend){
	cli();
	PINB |= 0b00010000;


	#define TIME_BETWEEN_CONTROL_PULSES 18 + 18
	if(commandToSend.frameControl & 0b100){
		talkOne_S();
	}else{
		talkZero_S();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_CONTROL_PULSES);

	if(commandToSend.frameControl & 0b10){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_CONTROL_PULSES);

	if(commandToSend.frameControl & 0b1){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_CONTROL_PULSES);








	//DATA------------------------------------------------------------------------
	#define TIME_BETWEEN_DATA_PULSES 18 + 18
	if(commandToSend.frameData & (1 << 7)){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(commandToSend.frameData & (1 << 6)){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(commandToSend.frameData & (1 << 5)){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(commandToSend.frameData & (1 << 4)){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(commandToSend.frameData & (1 << 3)){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(commandToSend.frameData & (1 << 2)){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(commandToSend.frameData & (1 << 1)){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	if(commandToSend.frameData & (1 << 0)){
		talkOne();
	}else{
		talkZero();
	}
	__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);

	PINB |= 0b00010000;
	sei();
}


#define PIN_1_ON_MACRO PORTD |= (0x01 << TALK_PIN_1);
#define PIN_1_OFF_MACRO PORTD &= ~(0x01 << TALK_PIN_1);
#define PIN_0_ON_MACRO PORTD |= (0x01 << TALK_PIN_0);
#define PIN_0_OFF_MACRO PORTD &= ~(0x01 << TALK_PIN_0);


#define PIN_1_FLIP 0b10
#define PIN_0_FLIP 0b01

//pulse duration basically
#define TIME_BETWEEN_PULSES 14 + 4
void talkOne(){
	PIN_1_ON_MACRO
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = PIN_1_FLIP << 4; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PIN_0_OFF_MACRO
}
void talkZero(){
	PIN_0_ON_MACRO
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = PIN_0_FLIP << 4; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PIN_1_OFF_MACRO
}

void talkOne_S(){
	PIN_1_ON_MACRO
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = PIN_1_FLIP << 4; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = PIN_0_FLIP << 4; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = PIN_1_FLIP << 4; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PIN_0_OFF_MACRO
}
void talkZero_S(){
	PIN_0_ON_MACRO
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = PIN_0_FLIP << 4; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = PIN_1_FLIP << 4; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PORTD = PIN_0_FLIP << 4; //flip both at the same time
	__builtin_avr_delay_cycles(TIME_BETWEEN_PULSES);
	PIN_1_OFF_MACRO
}
