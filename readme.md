
setup:
 - serial
 - interrupts

you send 3 bytes and receive 3, last is always \n or char number 10 in the ascii table

# made up codes
1XX are for extra messages you can send to the device, like asking it to just send the message it just received
2XX are for extra messages you can receive from the device  like errors
- 101: send back the last message you received
- 201: received message buffer overflowed, answers with 201 and the last byte it received