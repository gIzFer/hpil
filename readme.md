# HP-IL bridge/translator to USB/Serial

## What does it do
So what does this mean? The device simply takes what it receives from the HP-IL side and sends it over serial. Same in the other direction. sends 2 bytes, the `command` and the `data` terminated with a `'\n'` (ascii 10) character

## Why
I had a 3468A and wanted to control it from my computer and make the electronics circuit simple an easy and of parts everyone could have at hand. Difficulty was relayed onto the firmware, but even so was kept simple without implementing the state machines of the protocol.

## Materials needed
- Atmega328p
- 2 NPN and 4 PNP transistors. 2 PNP handle the input and the other 2 and 2 are for a push pull arrangement to send data. I sprinkled on them some diodes to speed them up.
- Ethernet transformer. Fastest pulses are around 1µs and an ethernet transformer seems to be able to handle that quite well. You can find them in every device having a rj45 port, old computers, routers, switches, etc.



## Added made up codes

### 1XX

They are for extra messages you can send to the device, like asking it to just send the message it just received

#### 1XX List of codes
- 101: send back the last message you received

### 2XX

They are for extra messages you can receive from the device  like errors

#### 2XX List of codes
- 201: received message buffer overflowed, answers with 201 and the last byte it received



# todo:
	- read pins instead of delays (NOPs) when talking to be able to read answer at the same time im writing