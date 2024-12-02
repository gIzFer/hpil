#include "talker.h"

#define OPTIMIZE_PARAMS GCC unroll

void setupTalker(){
	//configure pins as outputs
	DDRD |= (1 << TALK_PIN_1);
	DDRD |= (1 << TALK_PIN_0);
}

//every delay is 62.5ns
#define TIME_BETWEEN_CONTROL_PULSES 18 + 18 // 2.25µs 36 -> 7 read as delay (thinking of reading simultaneously as writting)s
#define TIME_BETWEEN_DATA_PULSES 18 + 18
//pulse duration basically
#define PULSE_DURATION 14 + 4 // 1.125 µs //18 -> 4 reads as delay (thinking of reading simultaneously as writting)s

void talk(struct command commandToSend){
	cli(); //stop interrupts while talking
	#ifdef DEBUG_PIN
		PINB |= 0b00010000;
	#endif


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


	for (int8_t i = 7; i >= 0; i--) {
		if(commandToSend.frameData & (1 << i)){
			talkOne();
		}else{
			talkZero();
		}
		__builtin_avr_delay_cycles(TIME_BETWEEN_DATA_PULSES);
	}
	#ifdef DEBUG_PIN
		PINB |= 0b00010000;
	#endif
	sei(); //resume interrupts after talkings
}


#define PIN_1_ON_MACRO TALK_PIN_PORT |= (1 << TALK_PIN_1);
#define PIN_1_OFF_MACRO TALK_PIN_PORT &= ~(1 << TALK_PIN_1);
#define PIN_0_ON_MACRO TALK_PIN_PORT |= (1 << TALK_PIN_0);
#define PIN_0_OFF_MACRO TALK_PIN_PORT &= ~(1 << TALK_PIN_0);
#define PIN_1_FLIP TALK_PIN_PORT = (1 << TALK_PIN_1);
#define PIN_0_FLIP TALK_PIN_PORT = (1 << TALK_PIN_0);


void talkOne(){
	PIN_1_ON_MACRO
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_0_FLIP //flip both at the same time
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_0_OFF_MACRO
}
void talkZero(){
	PIN_0_ON_MACRO
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_1_FLIP //flip both at the same time
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_1_OFF_MACRO
}

void talkOne_S(){
	PIN_1_ON_MACRO
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_0_FLIP //flip both at the same time
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_1_FLIP //flip both at the same time
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_0_FLIP //flip both at the same time
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_0_OFF_MACRO
}
void talkZero_S(){
	PIN_0_ON_MACRO
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_1_FLIP //flip both at the same time
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_0_FLIP //flip both at the same time
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_1_FLIP //flip both at the same time
	__builtin_avr_delay_cycles(PULSE_DURATION);
	PIN_1_OFF_MACRO
}
