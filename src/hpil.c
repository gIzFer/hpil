#include "hpil.h"

void hpil_handle(){

	//commandParsed is set from uart
	if(commandParsed == false){
		commandParsed = true;

		//sendByte('c');
		//sendByte(command[0]);
		//sendByte(command[1]);
		//sendByte('\n');
		struct command messageToSend;
		if(command[0] == 53){//command code 53 is reply last message received
			messageToSend.frameControl = frameControl;
			messageToSend.frameData = frameData;
		}else{
			messageToSend = messages[(uint8_t) command[0]];
			messageToSend.frameData |= (command[1] & messages[(uint8_t) command[0]].paramBits);
		}

		//sendByte('s');
		//sendByte(messageToSend.frameControl);
		//sendByte(messageToSend.frameData);
		//sendByte('\n');

		talk(messageToSend);
	}

}
