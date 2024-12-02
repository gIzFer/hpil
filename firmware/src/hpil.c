#include "hpil.h"

struct command messageToSend;
void hpil_handle(){

	//commandParsed is set from uart
	if(uart_commandParsed == false){
		uart_commandParsed = true;

		//sendByte('c');
		//sendByte(command[0]);
		//sendByte(command[1]);
		//sendByte('\n');
		if(uart_command[0] == 101){//custom command code 201 is reply last message receive
			messageToSend.frameControl = frameControl;
			messageToSend.frameData = frameData;
		}else{
			messageToSend = messages[(uint8_t) uart_command[0]];
			messageToSend.frameData |= (uart_command[1] & messages[(uint8_t) uart_command[0]].paramBits);
		}

		//sendByte('s');
		//sendByte(messageToSend.frameControl);
		//sendByte(messageToSend.frameData);
		//sendByte('\n');

		talk(messageToSend);
	}

}
