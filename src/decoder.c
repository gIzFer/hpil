#include "decoder.h"
#include <avr/interrupt.h>

#define PULSES_ARRAY_SIZE 70

volatile uint8_t pulses[PULSES_ARRAY_SIZE];
volatile uint8_t firstBitInterrupt; // signal received, queue decoding


uint8_t dataAvailable;
uint8_t frameControl;
uint8_t frameData;



/*
#PULSES_ARRAY_SIZE
for n in range(200):
    print("r({})".format(n), end="")
*/


//24 pulses, 3 samples per pulse: 72
//duration is ~50us, 50/72
#define r(n) pulses[n] = PIND;__builtin_avr_delay_cycles(7);
#define SAMPLE_SIGNAL r(0)r(1)r(2)r(3)r(4)r(5)r(6)r(7)r(8)r(9)r(10)r(11)r(12)r(13)r(14)r(15)r(16)r(17)r(18)r(19)r(20)r(21)r(22)r(23)r(24)r(25)r(26)r(27)r(28)r(29)r(30)r(31)r(32)r(33)r(34)r(35)r(36)r(37)r(38)r(39)r(40)r(41)r(42)r(43)r(44)r(45)r(46)r(47)r(48)r(49)r(50)r(51)r(52)r(53)r(54)r(55)r(56)r(57)r(58)r(59)r(60)r(61)r(62)r(63)r(64)r(65)r(66)r(67)r(68)r(69)//r(70)r(71)r(72)r(73)r(74)r(75)r(76)r(77)r(78)r(79)r(80)r(81)r(82)r(83)r(84)r(85)r(86)r(87)r(88)r(89)r(90)r(91)r(92)r(93)r(94)r(95)r(96)r(97)r(98)r(99)r(100)r(101)r(102)r(103)r(104)r(105)r(106)r(107)r(108)r(109)r(110)r(111)r(112)r(113)r(114)r(115)r(116)r(117)r(118)r(119)r(120)r(121)r(122)r(123)r(124)r(125)r(126)r(127)r(128)r(129)r(130)r(131)r(132)r(133)r(134)r(135)r(136)r(137)r(138)r(139)r(140)r(141)r(142)r(143)r(144)r(145)r(146)r(147)r(148)r(149)r(150)r(151)r(152)r(153)r(154)r(155)r(156)r(157)r(158)r(159)r(160)r(161)r(162)r(163)r(164)r(165)r(166)r(167)r(168)r(169)r(170)r(171)r(172)r(173)r(174)r(175)r(176)r(177)r(178)r(179)r(180)r(181)r(182)r(183)r(184)r(185)r(186)r(187)r(188)r(189)r(190)r(191)r(192)r(193)r(194)r(195)r(196)r(197)r(198)r(199)



//starts by 1
ISR(INT0_vect){
	PINB |= 0b00010000;
	SAMPLE_SIGNAL
	PINB |= 0b00010000;

	firstBitInterrupt = 1;
	EIMSK &= 0b00; //disable interrupts
}

//starts by 0
ISR(INT1_vect){
	PINB |= 0b00010000;
	SAMPLE_SIGNAL
	PINB |= 0b00010000;

	firstBitInterrupt = 2;
	EIMSK &= 0b00; //disable interrupts
}

void sample(){
	cli();
	PINB |= 0b00010000;
	SAMPLE_SIGNAL
	//for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
	//	pulses[i] = PIND;
	//}
	PINB |= 0b00010000;

	firstBitInterrupt = 2;
	//EIMSK &= 0b00; //disable interrupts
	sei();
}
void decodeFrame(){
	if(firstBitInterrupt != 0){
		PINB |= 0b00010000;
		cli();

		//check if empty to save time
		uint8_t emptyCheck = 0;
		for (uint8_t i = 0; i < 5; i++) {//unlikely any message starts with 16 zeroes ?
			if((pulses[i] & 0b00001100) != 0){
				emptyCheck = 1;
				//sendByte(pulses[i] & 0b00001100);
				//sendByte('c');
				break;
			}
		}
		/*sendByte('\n');
		sendByte('x');
		sendByte('x');
		sendByte('x');
		sendByte('\n');
		for (uint8_t i = 0; i < PULSES_ARRAY_SIZE; i++) {
			sendByte(pulses[i] + 48);
		}
		sendByte('\n');*/

		if(emptyCheck != 0){


			//track where I am in the frame decoding progress, 1-12 (0-11)
			uint8_t frameDecodingPos = 1;
			//already have the first bit from which ISR triggered

			//firstBitInterrupt=1 if interrupt was on 0
			//firstBitInterrupt=2 if interrupt was on 1
			//frameControl = (firstBitInterrupt == 1 ? 0 : 1) << 2;
			//control = 0;
			frameData = 0;


			uint8_t dataPair_[2];
			memset(dataPair_, 0, 2);
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

				//sendByte(pulse+48);


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
								frameControl = 255;
							}

							//sendByte('(');
							//sendByte(procesedBit+48);
							//sendByte(')');


							//sendByte('<');
							//sendByte(procesedBit+48);
							//sendByte('>');
							//add it to the result
							//frame decoding pos is in array, the constant is a total value, gotta -1 it
							if(frameDecodingPos < (FRAME_CONTROL_BITS)){
								frameControl |= procesedBit << (FRAME_CONTROL_BITS - frameDecodingPos - 1);
								frameDecodingPos++;
							}else{//its data
								frameData |= procesedBit << (FRAME_CONTROL_BITS + FRAME_DATA_BITS - frameDecodingPos - 1);
								frameDecodingPos++;
							}


						}else{
							frameControl = 255;
						}
					}
				}else{ //pulse was idle, so a new pulse can start with the same digit as the prev one. Set the previous value as idle and so we can start a new pair in the next loop
					dataPair_val_prevPair = 3;
				}
			}//end of for




			dataAvailable = 1;

			//sendByte('L');

			//char message_[32];
			//sprintf(message_, "%d - %d\r\n", frameControl, frameData);
			//sendStr(message_);


			//sendByte('\n');
			//sendByte(frameControl);
			sendByte(frameData);
			//sendByte(firstBitInterrupt);

			//sendByte('\n');





		}//end of empty check

		//enable interrupts
		firstBitInterrupt = 0;
		//clear interrupt flags
		//EIFR &= 0b00;
		//SREG &= ~(1<<7);
		//EIMSK |= 0b11; //enable INT0 and INT1
		sei();
		PINB |= 0b00010000;




	}
}
