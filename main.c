
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include<avr/interrupt.h>
#include <stdio.h>
#include <string.h>


//#define VERBOSE

#define baudRate 115200
#define UBRR0_value (F_CPU/(baudRate*8))-1

#define LED PORTB4

#define PULSES_ARRAY_SIZE 64
volatile uint8_t pulses[PULSES_ARRAY_SIZE];

volatile uint8_t control = 0; // signal received, queue decoding

#define FINAL_DATA_SIZE 12
uint8_t finalData[FINAL_DATA_SIZE];
//starts by 1
ISR(INT0_vect){
	EIMSK &= 0b00;
	PINB |= 0b00010000;
	control = 1;
	for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
		pulses[i] = PIND;
	}
	PINB |= 0b00010000;

}

//starts by 0
ISR(INT1_vect){
	EIMSK &= 0b00;
	PINB |= 0b00010000;
	control = 2;
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
	EIMSK |= 1; //enable INT0

	EICRA |= 0b1100; //INT1 on rising edge
	EIMSK |= 0b10; //enable INT1

	//configure pin interrupts //faster to directly int0
	//PCICR |= 1 << 2; //enable PCIE0 to receive interrupt from pin 0 to 7
	//PCMSK2 |= 1 << 2; //select pins that trigger interrupt on pcicr

	SREG |= 1<<SREG_I; //global interurpt enable


	usartPrintStr("hi!\n\r");

  DDRB |= (1 << LED); // set pin 13  as out

	for(;;) {
		//PINB |= (1 << LED);
    //_delay_ms(1000);
		//USART0SendByte('a');

		//this is the signal compressed, removing duplicated data
		#define SIGNAL_DATA_SIZE PULSES_ARRAY_SIZE
		uint8_t signalData[SIGNAL_DATA_SIZE];


		if(control != 0){
			PINB |= 0b00010000;
			cli();
			if((pulses[0] & 0b00001100) != 0){ // message is not empty, must start by 1

				//prepare iteration to compress signal
				uint8_t signalData_currentNdx = 0;
				memset(signalData, 0, SIGNAL_DATA_SIZE);

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



					if(signalData_currentNdx != 0 && signalData_currentNdx < SIGNAL_DATA_SIZE){
						if(signalData[signalData_currentNdx - 1] != pulse){
							signalData[signalData_currentNdx] = pulse;
							signalData_currentNdx++;
						}
					}else if(signalData_currentNdx == 0){
						signalData[signalData_currentNdx] = pulse;
						signalData_currentNdx++;
					}






					//detailed printing
					#ifdef VERBOSE
					char message[4];
					sprintf(message, "%d", pulse);
					usartPrintStr(message);
					#endif




				}//end of for
				#ifdef VERBOSE
				USART0SendByte('\r');
				USART0SendByte('\n');
				#endif



				//print signalData
				#ifdef VERBOSE
				for (uint8_t i = 0; i < SIGNAL_DATA_SIZE; i++) {
					char message[4];
					sprintf(message, "%d", signalData[i]);
					usartPrintStr(message);
				}
				USART0SendByte('\r');
				USART0SendByte('\n');
				#endif


				//decode the deduplicated data
				//the first bit is the trigger of the ISR
				//#define FINAL_DATA_SIZE 12
				//uint8_t finalData[FINAL_DATA_SIZE];
				finalData[0] = control & 0b1;
				uint8_t finalData_ndx = 1; //start on one, the first bit is set by the control var
				uint8_t dataPair[2];
				uint8_t dataPair_ndx = 0;

				for (uint8_t i = 0; i < SIGNAL_DATA_SIZE; i++) {
					if(signalData[i] == 1 || signalData[i] == 0){//ignore idles
						if(dataPair_ndx == 0){//first val
							dataPair[dataPair_ndx] = signalData[i];
							dataPair_ndx++;
						}else if(dataPair_ndx == 1){
							dataPair[dataPair_ndx] = signalData[i];
							dataPair_ndx = 0;

							if(finalData_ndx < FINAL_DATA_SIZE + 1){
								if(dataPair[0] == 0 && dataPair[1] == 1){
									finalData[finalData_ndx] = 0;
									finalData_ndx++;
								}else if(dataPair[0] == 1 && dataPair[1] == 0){
									finalData[finalData_ndx] = 1;
									finalData_ndx++;
								}else{//prob read a 3, idle between pulses for whatev reason, push back and keep trying
									finalData[0] = finalData[1];
								}
							}else{
								finalData[0] = 7; //put a 7 to let know theres an error
								finalData[FINAL_DATA_SIZE - 1] = 7; //put a 7 to let know theres an error
							}


						}
					}
				}//end of decoding for()

			}
			control = 0;
			EIMSK |= 0b11; //enable INT0 and INT1
			sei();
			PINB |= 0b00010000;



		//print decoded data
		if(finalData[2] == 0){

			char message[4];
			for (uint8_t i = 0; i < FINAL_DATA_SIZE; i++) {
				if(i == 2 || i == 4){
					USART0SendByte(' ');
				}
				USART0SendByte(finalData[i] + 48);
				//sprintf(message, "%d", finalData[i]);
				//usartPrintStr(message);
			}
			USART0SendByte('\r');
			USART0SendByte('\n');

			USART0SendByte('\r');
			USART0SendByte('\n');

		}





		}



  }

}
