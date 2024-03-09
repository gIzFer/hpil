#pragma once

#include <stdint.h>
struct command{
	uint8_t id;
	uint8_t frameControl;
	uint8_t frameData;
};

extern const struct command messages[53];

//Data Or End class
extern const struct command DAB; // DAta Byte
extern const struct command END; // data END



//CoMmanD class

//Addressed Comand subGroup
extern const struct command NUL; // NULl command
extern const struct command GTL; // Go To Local
extern const struct command SDC; // Selected Device Clear
extern const struct command PPD; // Parallel Poll Disable
extern const struct command GET; // Group Execute Trigger
extern const struct command ELN; // Enable Listener Not ready
extern const struct command PPE; // Parallel Poll Enable
extern const struct command DDL; // Device Dependent Listener
extern const struct command DDT; // Device Dependent Talker

//Universal Command subGroup
extern const struct command NOP; // No OPeration
extern const struct command LLO; // Local LOckout
extern const struct command DCL; // Device CLear
extern const struct command PPU; // Parallel Poll Unconfigure
extern const struct command EAR; // Enabel Asynchronous Requests
extern const struct command IFC; // InterFace Clear
extern const struct command REN; // Remote ENable
extern const struct command NRE; // Not Remote ENable
extern const struct command AAU; // Auto Address Unconfigure
extern const struct command LPD; // Loop Power Down

//Listen Address subGroup
extern const struct command LAD; // Listen ADdress
extern const struct command MLA; // My Listen Address
extern const struct command UNL; //UNListen

//Listen Address subGroup
extern const struct command TAD; // Talk ADdress
extern const struct command MTA; // My Talk Address
extern const struct command OTA; // Other Talk Address
extern const struct command UNT; //UNTalk

//Secondary Address subGroup
extern const struct command SAD; // Secondary ADdress
extern const struct command MSA; // My Secondary Address
extern const struct command OSA; // Other Secondary Address



//IDentifY class
extern const struct command IDY; // Secondary ADdress



//ReaDY class
extern const struct command RFC; //Ready For Command

//Addressed Ready subGroup
//End Of Transmission subSubGroup
extern const struct command ETO; // End of Transmission - Ok
extern const struct command ETE; // End of Transmission - Error

extern const struct command NRD; //Not Ready for Data

//Start Of Transmission subSubGroup
extern const struct command SDA; // Send DAta
extern const struct command SST; // Send STatus
extern const struct command SDI; // Send Device Id
extern const struct command SAI; // Send Accessory Id
extern const struct command TCT; // Take ConTrol


//Auto Address subGroup
extern const struct command AAD; // Auto ADdress
extern const struct command NAA; // Next Auto Address
extern const struct command IAA; // Illegal Auto Address

extern const struct command AEP; // Auto Extended Primary
extern const struct command IEP; // Illegal Extended Primary

extern const struct command ZES; // Zero Extended Secondary
extern const struct command AES; // Auto Extended Secondary
extern const struct command NES; // Next Extended Secondary
extern const struct command IES; // Illegal Extended Secondary

extern const struct command AMP; // Auto Multiple Primary
extern const struct command NMP; // Next Multiple Primary
extern const struct command IMP; // Illegal Multiple Primary





/*
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
*/