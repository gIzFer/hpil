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
/*
	if(dataAvailable != 0){
		dataAvailable = 0;
		//sendByte(frameData);
		//sendByte('>');
		//sendByte(frameControl);
		//sendByte(frameData);
		//sendByte('\n');

	if(commandSent.hasBeenReceivedBackFromLoop == false){
			commandSent.hasBeenReceivedBackFromLoop = true;
		}else{
			talk(frameControl, frameData);
		}
	}*/

	switch (protocolStage) {
	case IFC_STAGE:
		protocolStage++;
		sendByte('S');
		sendByte(protocolStage);
		configureTimeout(PRE_SCALER_256_1S);
		startTimeout();
		talk(IFC);
		break;
	case IFC_STAGE + 1:
		if(CHECK_TIMEOUT){//timeout occurred
			sendByte('T');
			startTimeout();
			talk(IFC);
		}else{
			if(dataAvailable != 0){
				dataAvailable = 0;
				//since IFCs are retransmitted inmediately we gotta assume whatever comes next, which will be cut is the IFC
				protocolStage++;
				stopTimeout();
				//not sure if frame Control can return with the SRQ bit on?
				//if(frameControl == IFC.frameControl && frameData == IFC.frameData){
				//	protocolStage++;
				//	stopTimeout();
				//}
			}
		}
		break;
	case RFC_STAGE:
		protocolStage++;
		sendByte('S');
		sendByte(protocolStage);
		configureTimeout(PRE_SCALER_1024_4S);
		startTimeout();
		talk(RFC);
		break;
	case RFC_STAGE + 1:
		if(CHECK_TIMEOUT){//timeout occurred
			//restart process
			startTimeout();
			protocolStage = 0;
			sendByte('R');
		}else{
				if(dataAvailable != 0){
					dataAvailable = 0;
					sendByte('D');
					//not sure if frame Control can return with the SRQ bit on?
					//if(frameControl == RFC.frameControl && frameData == RFC.frameData){
						protocolStage++;
						stopTimeout();
					//}
				}
			}
		break;
	case READY_STAGE:
		if(temp == 0){
			temp = 1;
				sendByte('A');
				sendByte('S');
				sendByte(protocolStage);
		}
		if(commandParsed == false){
			commandParsed = true;
			sendByte('_');

			if(command[0] == 'z'){//RFC
				sendByte('z');
				talk(RFC);
			}
			if(command[0] == 'x'){ //SDA send data
				sendByte('x');
				talk((struct command){255, 0b101, 0b01100000});
			}
			if(command[0] == 'c'){ //send last command back
				sendByte('c');
				talk((struct command){255, frameControl, frameData});
			}
			if(command[0] == 'v'){ //IFC
				sendByte('v');
				talk(IFC);
			}
			if(command[0] == 'b'){ //SST send status
				sendByte('b');
				talk((struct command){255, 0b101, 0b01100001});
			}
			if(command[0] == 'n'){ //NRE Not Remote Enable
				sendByte('n');
				talk((struct command){255, 0b100, 0b10010011});
			}
			if(command[0] == 'm'){ //REN  Remote Enable
				sendByte('m');
				talk((struct command){255, 0b100, 0b10010010});
			}


			if(command[0] == 'q'){//set as LAD
				sendByte('q');
				talk((struct command){255, 0b100, (0b100000 | 22)});
			}
			if(command[0] == 'w'){//UNListen
				sendByte('w');
				talk((struct command){255, 0b100, 0b00111111});
			}
			if(command[0] == 'e'){//set as TAD
				sendByte('e');
				talk((struct command){255, 0b100, (0b1000000 | 22)});
			}
			if(command[0] == 'r'){//UNTalk
				sendByte('r');
				talk((struct command){255, 0b100, 0b01011111});
			}


			if(command[0] == 'a'){ //char
				sendByte('a');
				talk((struct command){255, 0, 'D'});
			}
			if(command[0] == 's'){ //char
				sendByte('s');
				talk((struct command){255, 0, '2'});
			}
			if(command[0] == 'd'){ //char
				sendByte('d');
				talk((struct command){255, 0, 'A'});
			}
			if(command[0] == 'f'){ //end byte?
				sendByte('e');
				talk((struct command){255, 0b010, 0});
			}
			if(command[0] == 'g'){ //EOTransmission
				sendByte('f');
				talk((struct command){255, 0b101, 0b010000});
			}
			if(command[0] == 'h'){ //char
				sendByte('h');
				talk((struct command){255, 0, 'B'});
			}
			if(command[0] == 'j'){ //char
				sendByte('j');
				talk((struct command){255, 0, '1'});
			}
			if(command[0] == 'k'){ //char
				sendByte('k');
				talk((struct command){255, 0, 'F'});
			}

			/*if(command[0] == 'b'){
				uint8_t frameControl_ = 0b101;
				uint8_t frameData_ = 0;
				talk(frameControl_, frameData_);
				commandSent.hasBeenReceivedBackFromLoop = false;
				commandSent.frameControl = frameControl_;
				commandSent.frameData = frameData_;
			}*/
		}

		break;
	default:
		sendByte('W');
	}




/*
	if(commandParsed == false){
		//sendStr(command);
		commandParsed = true;

		if(command[0] == 'c'){ //AAD auto address NO USE
			uint8_t frameControl_ = 0b101;
			uint8_t frameData_ = 0b10000000;
			talk(frameControl_, frameData_);
			commandSent.hasBeenReceivedBackFromLoop = false;
			commandSent.frameControl = frameControl_;
			commandSent.frameData = frameData_;
		}





		if(command[0] == 'd'){ //LAD
			uint8_t frameControl_ = 0b100;
			uint8_t frameData_ = 0b001 | 22;
			talk(frameControl_, frameData_);
			commandSent.hasBeenReceivedBackFromLoop = false;
			commandSent.frameControl = frameControl_;
			commandSent.frameData = frameData_;
		}

		if(command[0] == 'e'){ //
			uint8_t frameControl_ = 0b000;
			uint8_t frameData_ = 'D';
			talk(frameControl_, frameData_);
			commandSent.hasBeenReceivedBackFromLoop = false;
			commandSent.frameControl = frameControl_;
			commandSent.frameData = frameData_;
		}

		if(command[0] == 'f'){ //
			uint8_t frameControl_ = 0b000;
			uint8_t frameData_ = '2';
			talk(frameControl_, frameData_);
			commandSent.hasBeenReceivedBackFromLoop = false;
			commandSent.frameControl = frameControl_;
			commandSent.frameData = frameData_;
		}

		if(command[0] == 'g'){ //
			uint8_t frameControl_ = 0b000;
			uint8_t frameData_ = 'W';
			talk(frameControl_, frameData_);
			commandSent.hasBeenReceivedBackFromLoop = false;
			commandSent.frameControl = frameControl_;
			commandSent.frameData = frameData_;
		}

		if(command[0] == 'h'){ //
			uint8_t frameControl_ = 0b010;
			uint8_t frameData_ = 'w';
			talk(frameControl_, frameData_);
			commandSent.hasBeenReceivedBackFromLoop = false;
			commandSent.frameControl = frameControl_;
			commandSent.frameData = frameData_;
		}
	}
*/



}