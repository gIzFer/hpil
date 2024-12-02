#!/usr/bin/bash


killall screen
avr-gcc -mmcu=atmega328p -Wall -Os -o build/main.elf main.c && avrdude -c arduino -p atmega328p -P /dev/ttyUSB0 -U flash:w:build/main.elf
