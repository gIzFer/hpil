#include "hpil.h"

//((16MHz/prescaler)^-1)*65535
//0b101 prescaler at /1024 - 4.19424s
//0b100 prescaler at /256 - 1.04856s
//0b011 prescaler at /64 - 0.26214s
#define PRE_SCALER_1024_4S 0b101
#define PRE_SCALER_256_1S 0b100
#define PRE_SCALER_64_260MS 0b011
#define CHECK_TIMEOUT (TIFR1 & (1 << TOV1)) != 0


#define COMMAND_QUEUE_SIZE 15
struct sentCommandState commandQueue[COMMAND_QUEUE_SIZE];
uint8_t commandQueue_ndx = 0;


//0 startup -> send IFC until it returns

//HP82166 if. spec. ch 5.4.1 Power up and Error Recovery
//num is init of the stage and num + 1 the actual stage
#define IFC_STAGE 0 //do: send IFCs every .26s until they make their way back
#define RFC_STAGE 2 //IFC received, send 1 RFC and wait 4s. if it does not return restart process
#define READY_STAGE 4 // loop initialized, ready for instructions
uint8_t protocolStage = IFC_STAGE;
void configureTimeout(uint8_t time){
	TCCR1B = time; //prescaler at 256: 1s to fill
}
void startTimeout(){
	TCNT1H = 0; //high first for writing
	TCNT1L	= 0;
	TIFR1 |= 1 << TOV1; //clear overflow flag
}
void stopTimeout(){
	TCCR1B = 0;
	TIFR1 |= 1 << TOV1; //clear overflow flag
}

void hpil_setup(){
	configureTimeout(PRE_SCALER_256_1S);
}
uint8_t temp = 0;
void hpil_handle(){

	//commandParsed is from uart
	if(commandParsed == false){
		commandParsed = true;

		//sendByte('c');
		//sendByte(command[0]);
		//sendByte(command[1]);
		//sendByte('\n');

		struct command messageToSend = messages[command[0]];
		messageToSend.frameData |= (command[1] & messages[command[0]].paramBits);

		//sendByte('s');
		//sendByte(messageToSend.frameControl);
		//sendByte(messageToSend.frameData);
		//sendByte('\n');

		talk(messageToSend);
	}

}
