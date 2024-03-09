#include "messages.h"
//class>subgroup>command



//Data Or End class
const struct command DAB = {0, 0b000, 0}; // DAta Byte
const struct command END = {1, 0b010, 0}; // data END



//CoMmanD class

//Addressed Comand subGroup
const struct command NUL = {2, 0b100, 0}; // NULl command
const struct command GTL = {3, 0b100, 0b00000001}; // Go To Local
const struct command SDC = {4, 0b100, 0b00000100}; // Selected Device Clear
const struct command PPD = {5, 0b100, 0b00000101}; // Parallel Poll Disable
const struct command GET = {6, 0b100, 0b00001000}; // Group Execute Trigger
const struct command ELN = {7, 0b100, 0b00001111}; // Enable Listener Not ready
const struct command PPE = {8, 0b100, 0b10000000}; // Parallel Poll Enable
const struct command DDL = {9, 0b100, 0b10100000}; // Device Dependent Listener
const struct command DDT = {10, 0b100, 0b11000000}; // Device Dependent Talker

//Universal Command subGroup
const struct command NOP = {11, 0b100, 0b00010000}; // No OPeration
const struct command LLO = {12, 0b100, 0b00010001}; // Local LOckout
const struct command DCL = {13, 0b100, 0b00010100}; // Device CLear
const struct command PPU = {14, 0b100, 0b00010101}; // Parallel Poll Unconfigure
const struct command EAR = {15, 0b100, 0b00011000}; // Enabel Asynchronous Requests
const struct command IFC = {16, 0b100, 0b10010000}; // InterFace Clear
const struct command REN = {17, 0b100, 0b10010010}; // Remote ENable
const struct command NRE = {18, 0b100, 0b10010011}; // Not Remote ENable
const struct command AAU = {19, 0b100, 0b10011010}; // Auto Address Unconfigure
const struct command LPD = {20, 0b100, 0b10011011}; // Loop Power Down

//Listen Address subGroup
const struct command LAD = {21, 0b100, (0b00100000 | 22)}; // Listen ADdress
const struct command MLA = {22, 0b100, 0b00100000}; // My Listen Address
const struct command UNL = {23, 0b100, 0b00111111}; //UNListen

//Listen Address subGroup
const struct command TAD = {24, 0b100, (0b01000000 | 22)}; // Talk ADdress
const struct command MTA = {25, 0b100, 0b01000000}; // My Talk Address
const struct command OTA = {26, 0b100, 0b01000000}; // Other Talk Address
const struct command UNT = {27, 0b100, 0b01011111}; //UNTalk

//Secondary Address subGroup
const struct command SAD = {28, 0b100, 0b01100000}; // Secondary ADdress
const struct command MSA = {29, 0b100, 0b01100000}; // My Secondary Address
const struct command OSA = {30, 0b100, 0b01100000}; // Other Secondary Address



//IDentifY class
const struct command IDY = {31, 0b110, 0}; // Secondary ADdress



//ReaDY class
const struct command RFC = {32, 0b101, 0}; //Ready For Command

//Addressed Ready subGroup
//End Of Transmission subSubGroup
const struct command ETO = {33, 0b101, 0b01000000}; // End of Transmission - Ok
const struct command ETE = {34, 0b101, 0b01000001}; // End of Transmission - Error

const struct command NRD = {35, 0b101, 0b01000010}; //Not Ready for Data

//Start Of Transmission subSubGroup
const struct command SDA = {36, 0b101, 0b01100000}; // Send DAta
const struct command SST = {37, 0b101, 0b01100001}; // Send STatus
const struct command SDI = {38, 0b101, 0b01100010}; // Send Device Id
const struct command SAI = {39, 0b101, 0b01100011}; // Send Accessory Id
const struct command TCT = {40, 0b101, 0b01100100}; // Take ConTrol


//Auto Address subGroup
const struct command AAD = {41, 0b101, 0b10000000}; // Auto ADdress
const struct command NAA = {42, 0b101, 0b10000000}; // Next Auto Address
const struct command IAA = {43, 0b101, 0b10011111}; // Illegal Auto Address

const struct command AEP = {44, 0b101, 0b10100000}; // Auto Extended Primary
const struct command IEP = {45, 0b101, 0b10111111}; // Illegal Extended Primary

const struct command ZES = {46, 0b101, 0b11000000}; // Zero Extended Secondary
const struct command AES = {47, 0b101, 0b11000000}; // Auto Extended Secondary
const struct command NES = {48, 0b101, 0b11000000}; // Next Extended Secondary
const struct command IES = {49, 0b101, 0b11011111}; // Illegal Extended Secondary

const struct command AMP = {50, 0b101, 0b11100000}; // Auto Multiple Primary
const struct command NMP = {51, 0b101, 0b11100000}; // Next Multiple Primary
const struct command IMP = {52, 0b101, 0b11111111}; // Illegal Multiple Primary


const struct command messages[] = {
DAB, END,

NUL, GTL, SDC, PPD, GET, ELN, PPE, DDL, DDT,
NOP, LLO, DCL, PPU, EAR, IFC, REN, NRE, AAU, LPD,
LAD, MLA, UNL,
TAD, MTA, OTA, UNT,
SAD, MSA, OSA,

IDY,

RFC,
ETO, ETE,
NRD,
SDA, SST, SDI, SAI, TCT,
AAD, NAA, IAA,
AEP, IEP,
ZES, AES, NES, IES,
AMP, NMP, IMP
};
