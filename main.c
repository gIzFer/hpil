
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include<avr/interrupt.h>
#include <stdio.h>
#include <string.h>


#define VERBOSE

#define baudRate 115200
#define UBRR0_value (F_CPU/(baudRate*8))-1

#define LED PORTB4

#define PULSES_ARRAY_SIZE 55
volatile uint8_t pulses[PULSES_ARRAY_SIZE];

volatile uint8_t firstBitInterrupt = 0; // signal received, queue decoding


#define FRAME_CONTROL_BITS 4
#define FRAME_DATA_BITS 8
#define FRAME_SIZE (FRAME_CONTROL_BITS + FRAME_DATA_BITS)
uint8_t control = 0;
uint8_t data = 0;






//starts by 1
ISR(INT0_vect){
	EIMSK &= 0b00;
	PINB |= 0b00010000;
	firstBitInterrupt = 1;
	for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
		pulses[i] = PIND;
	}
	PINB |= 0b00010000;

}

//starts by 0
ISR(INT1_vect){
	EIMSK &= 0b00;
	PINB |= 0b00010000;
	firstBitInterrupt = 2;
	for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
		pulses[i] = PIND;
	}
	PINB |= 0b00010000;

}

void USART0SendByte(uint8_t u8Data){
	while(!(UCSR0A&(1<<UDRE0))){};
	UDR0 = u8Data;
}
void usartPrintStr(char* str){
	for (uint8_t i = 0; i < strlen(str); i++){
		USART0SendByte(str[i]);
	}
}


int main(void) {
	//enable tx
	UCSR0A |= 1<<U2X0;
	UCSR0B |= 1<<TXEN0;
	UBRR0H = (uint8_t)(UBRR0_value) >> 8;
	UBRR0L = (uint8_t)UBRR0_value;

	//set interrupt
	EICRA |= 0b11; //INT0 on rising edge
	EIMSK |= 0b01; //enable INT0

	EICRA |= 0b1100; //INT1 on rising edge
	EIMSK |= 0b10; //enable INT1

	//configure pin interrupts //faster to directly int0
	//PCICR |= 1 << 2; //enable PCIE0 to receive interrupt from pin 0 to 7
	//PCMSK2 |= 1 << 2; //select pins that trigger interrupt on pcicr

	SREG |= 1<<SREG_I; //global interurpt enable


	usartPrintStr("hi!\n\r");

  DDRB |= (1 << LED); // set pin 13  as out

	for(;;) {


		if(firstBitInterrupt != 0){
			PINB |= 0b00010000;
			cli();

			//check if empty to save time
			uint8_t emptyCheck = 0;
			for (size_t i = 0; i < 5; i++) {//unlikely any message starts with 5 zeroes ?
				emptyCheck += pulses[i] & 0b00001100;
			}


			if(emptyCheck != 0){ // message is not empty, must start by 1
				//USART0SendByte(firstBitInterrupt + 48);
				//USART0SendByte('_');






				//track where I am in the frame decoding progress, 1-12 (0-11)
				uint8_t frameDecodingPos = 1;
				//already have the first bit from which ISR triggered
				control = (firstBitInterrupt & 0b10) << 2;
				//control = 0;
				data = 0;



				uint8_t dataPair_[2];
				uint8_t dataPair_ndx_ = 0;
				uint8_t dataPair_val_prevPair = 0;


				for(int i = 0; i < PULSES_ARRAY_SIZE; i++){
					uint8_t pulse = pulses[i] & 0b00001100;

					//pulse translating;

					if(pulse == 8){//8 means 0
						pulse = 0;
					}else if(pulse == 4){//4 means 1
						pulse = 1;
					}else if(pulse == 12){//12 shouldn't happen, signals overlapped, should be treated like an idle I guess
						pulse = 2;
					}else{//0 idle
						pulse = 3;
					}




					//detailed printing
					#ifdef VERBOSE
					//char message[10];
					//sprintf(message, "@%d@", pulse);
					//usartPrintStr(message);
					#endif


					if(pulse == 1 || pulse == 0){//ignore idles
						//first val and its not the same as the inmediately previous one, it needs a 3 between to equal ones to be valid
						//abusing order of check so it doesnt evaluate pulses[i - 1] on the first run as that would crash
						if(dataPair_ndx_ == 0 && pulse != dataPair_val_prevPair){
							dataPair_[0] = pulse;
							dataPair_ndx_++;
						}else if(dataPair_ndx_ == 1 && dataPair_[0] != pulse){ //not repeated, for deduplication of oversampling
							dataPair_val_prevPair = pulse;
							dataPair_[1] = pulse;
							dataPair_ndx_ = 0;


							//here I must have a pair of 0 and 1
							if(frameDecodingPos < FRAME_SIZE){
								//get the value of the bit pair
								uint8_t procesedBit = 0;
								if(dataPair_[0] == 0 && dataPair_[1] == 1){
									procesedBit = 1;
								}else if(dataPair_[0] == 1 && dataPair_[1] == 0){
									procesedBit = 0;
								}else{//no?
									control = 255;
								}




								//add it to the result
								//frame decoding pos is in array, the constant is a total value, gotta -1 it
								if(frameDecodingPos < (FRAME_CONTROL_BITS)){
									control |= procesedBit << (FRAME_CONTROL_BITS - frameDecodingPos - 1);
									frameDecodingPos++;
								}else{//its data
									data |= procesedBit << (FRAME_CONTROL_BITS + FRAME_DATA_BITS - frameDecodingPos - 1);
									frameDecodingPos++;
								}


							}else{
								control = 255;
							}


						}
					}else{ //idle, so a new pulse can start with the same digit as the prev one
						dataPair_val_prevPair = 3;
					}







				}//end of for




				#ifdef VERBOSE
				//USART0SendByte('\r');
				//USART0SendByte('\n');
				#endif




			}
			firstBitInterrupt = 0;
			EIMSK |= 0b11; //enable INT0 and INT1
			sei();
			PINB |= 0b00010000;


		//char message_[32];
		//sprintf(message_, "%d - %d\r\n", control, data);
		//usartPrintStr(message_);


		//USART0SendByte(control);
		USART0SendByte(data);

		//USART0SendByte('\n');





		}



  }

}
