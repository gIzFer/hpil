#include "talker.h"
//class>subgroup>command

const struct hpil_messages{
	const struct class_DOE{ //Data Or End class
		const struct command DAB = {1, 0b000, 0}; // DAta Byte
		const struct command END = {1, 0b010, 0}; // data END
	} DOE;
	const struct class_CMD{ //CoMmanD class
		const struct subgroup_ACG{ //Addressed Comand Group
			const struct command NUL = {1, 0b100, 0}; // NULl command
			const struct command GTL = {1, 0b100, 0b00000001}; // Go To Local
			const struct command SDC = {1, 0b100, 0b00000100}; // Selected Device Clear
			const struct command PPD = {1, 0b100, 0b00000101}; // Parallel Poll Disable
			const struct command GET = {1, 0b100, 0b00001000}; // Group Execute Trigger
			const struct command ELN = {1, 0b100, 0b00001111}; // Enable Listener Not ready
			const struct command PPE = {1, 0b100, 0b10000000}; // Parallel Poll Enable
			const struct command DDL = {1, 0b100, 0b10100000}; // Device Dependent Listener
			const struct command DDT = {1, 0b100, 0b11000000}; // Device Dependent Talker
		} ACG;
		const struct subgroup_UCG{ //Universal Command Group
			const struct command NOP = {1, 0b100, 0b00010000}; // No OPeration
			const struct command LLO = {1, 0b100, 0b00010001}; // Local LOckout
			const struct command DCL = {1, 0b100, 0b00010100}; // Device CLear
			const struct command PPU = {1, 0b100, 0b00010101}; // Parallel Poll Unconfigure
			const struct command EAR = {1, 0b100, 0b00011000}; // Enabel Asynchronous Requests
			const struct command IFC = {1, 0b100, 0b10010000}; // InterFace Clear
			const struct command REN = {1, 0b100, 0b10010010}; // Remote ENable
			const struct command NRE = {1, 0b100, 0b10010011}; // Not Remote ENable
			const struct command AAU = {1, 0b100, 0b10011010}; // Auto Address Unconfigure
			const struct command LPD = {1, 0b100, 0b10011011}; // Loop Power Down
		} UCG;
		const struct subgroup_LAG{ //Listen Address Group
			const struct command LAD = {1, 0b100, (0b00100000 | 22)}; // Listen ADdress
			const struct command MLA = {1, 0b100, 0b00100000}; // My Listen Address
			const struct command UNL = {1, 0b100, 0b00111111}; //UNListen
		} LAG;
		const struct subgroup_TAG{ //Listen Address Group
			const struct command TAD = {1, 0b100, (0b01000000 | 22)}; // Talk ADdress
			const struct command MTA = {1, 0b100, 0b01000000}; // My Talk Address
			const struct command OTA = {1, 0b100, 0b01000000}; // Other Talk Address
			const struct command UNT = {1, 0b100, 0b01011111}; //UNTalk
		} TAG;
		const struct subgroup_SAG{ //Secondary Address Group
			const struct command SAD = {1, 0b100, 0b01100000}; // Secondary ADdress
			const struct command MSA = {1, 0b100, 0b01100000}; // My Secondary Address
			const struct command OSA = {1, 0b100, 0b01100000}; // Other Secondary Address
		} SAG;
	} CMD;
	const struct class_IDY{ //IDentifY
		const struct command IDY = {1, 0b110, 0}; // Secondary ADdress
	} IDY;
	const struct class_RDY{ //ReaDY
		const struct command RFC = {1, 0b101, 0}; //Ready For Command
		const struct subgroup_ARG{ //Addressed Ready Group
			const struct subSubgroup_EOT{ //End Of Transmission
				const struct command ETO = {1, 0b101, 0b01000000}; // End of Transmission - Ok
				const struct command ETE = {1, 0b101, 0b01000001}; // End of Transmission - Error
			} EOT;
			const struct command NRD = {1, 0b101, 0b01000010}; //Not Ready for Data
			const struct subSubgroup_SOT{ //Start Of Transmission
				const struct command SDA = {1, 0b101, 0b01100000}; // Send DAta
				const struct command SST = {1, 0b101, 0b01100001}; // Send STatus
				const struct command SDI = {1, 0b101, 0b01100010}; // Send Device Id
				const struct command SAI = {1, 0b101, 0b01100011}; // Send Accessory Id
				const struct command TCT = {1, 0b101, 0b01100100}; // Take ConTrol
			} SOT;
		} ARG;
		const struct subgroup_AAG{ //Auto Address Group
			const struct command AAD = {1, 0b101, 0b10000000}; // Auto ADdress
			const struct command NAA = {1, 0b101, 0b10000000}; // Next Auto Address
			const struct command IAA = {1, 0b101, 0b10011111}; // Illegal Auto Address

			const struct command AEP = {1, 0b101, 0b10100000}; // Auto Extended Primary
			const struct command IAA = {1, 0b101, 0b10111111}; // Illegal Extended Primary

			const struct command ZES = {1, 0b101, 0b11000000}; // Zero Extended Secondary
			const struct command AES = {1, 0b101, 0b11000000}; // Auto Extended Secondary
			const struct command NES = {1, 0b101, 0b11000000}; // Next Extended Secondary
			const struct command IAA = {1, 0b101, 0b11011111}; // Illegal Extended Secondary

			const struct command AMP = {1, 0b101, 0b11100000}; // Auto Multiple Primary
			const struct command NMP = {1, 0b101, 0b11100000}; // Next Multiple Primary
			const struct command IMP = {1, 0b101, 0b11111111}; // Illegal Multiple Primary
		} AAG;
	} RDY;
} hpil_message;

void setupTalker(){
		DDRD |= (1 << TALK_PIN_1);
	DDRD |= (1 << TALK_PIN_0);
}

void talk(struct command commandToSend){
	cli();
	PINB |= 0b00010000;
	//sendByte('#');
	//sendByte('\n');
	//sendByte(commandToSend.frameControl);
	//sendByte(commandToSend.frameData);
	//sendByte('\n');

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







	//for is too slow
	/*for (uint8_t i = 8; i >= 1; i--) {
		if(commandToSend.frameData & 0b1 << i){
			talkOne();
		}else{
			talkZero();
		}
		//__builtin_avr_delay_cycles(12);
	}*/

	//__builtin_avr_delay_cycles(16*100);
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
