# HP-IL bridge/translator to USB/Serial

## What does it do
So what does this do? The device simply takes what it receives from the HP-IL side and sends it over serial. Same in the other direction. sends 2 bytes, the `command` and the `data` terminated with a `'\n'` (ascii 10) character

## Why
I had a 3468A and wanted to control it from my computer and make the electronics circuit simple an easy and of parts everyone could have at hand. Difficulty was relayed onto the firmware, but even so was kept simple without implementing the state machines of the protocol.

## Materials needed
- Atmega328p
- 2 NPN and 4 PNP transistors. 2 PNP handle the input and the other 2 and 2 are for a push pull arrangement to send data. I sprinkled on them some diodes to speed them up.
- Ethernet transformer. Fastest pulses are around 1µs and an ethernet transformer seems to be able to handle that quite well. You can find them in every device having a rj45 port, old computers, routers, switches, etc.

[client docs](./client/readme.md)

## Added made up codes

### 1XX

They are for extra messages you can send to the device, like asking it to just send the message it just received

#### 1XX List of codes
- 101: send back the last message received
- 102: get calibration data of the HP3468 dmm
- 103: get firmware version
- 104: set calibration value of the HP3468 dmm. Has 5 parameters: first one is the range setting to set. Since there are 12 ranges and two values per range the even values are offsets and odd values are gains. i.e.: `104 6 0 0 0 0` set the offset of the fourth range to 0.i.e.: `104 1 0 0 0 0` set the gain of the first range to 0.
- 105: print processed calibration values of the HP3468 dmm
- 106: print a quick read of the HP3468 dmm

### 2XX

They are for extra messages you can receive from the device  like errors

#### 2XX List of codes
- 201: received message buffer overflowed, answers with 201 and the last byte it received
- 202: in command 104, trying to set a cal value the value was more than 23; there are only 12 ranges with 2 values per range (offset and gain), 0 to 23.



# todo:
- read pins instead of delays (NOPs) when talking to be able to read answer at the same time im writing
- add images to readme
- add "console mode" so no need for the python client, to be able to directly send the commands to it
- send and write calibration data