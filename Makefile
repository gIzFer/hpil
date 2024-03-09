

#common
OUT=build/main.elf

#compiler
CC=avr-gcc
CFLAGS=-mmcu=atmega328p -Ofast -Werror -Wall -Wextra -funroll-loops -fvariable-expansion-in-unroller --param=min-pagesize=0 -ffunction-sections -fdata-sections -Wl,--gc-sections,--print-gc-sections


SRC_FOLDER=src
BUILD_FOLDER=build

SRC=$(shell find $(SRC_FOLDER) -name '*.c')

#programmer
PG=avrdude
PFLAGS=-c arduino -p atmega328p

#make flash U=2    to flash to USB2
U=1
USB=/dev/ttyUSB$(U)


#avr-gcc -mmcu=atmega328p -Wall -Os -I src/ -c -o main.o src/main.c
#avr-gcc -mmcu=atmega328p -Wall -Os -I src/ -c -o interrupts.o src/interrupts.c
#avr-gcc -mmcu=atmega328p -Wall -Os -I src/ -o prog  main.o interrupts.o

#OBJECTS := $(addprefix $(BUILD_FOLDER)/,$(SRC:%.c=%.o))

OBJECTS := $(patsubst $(SRC_FOLDER)/%, $(BUILD_FOLDER)/%, $(SRC:%.c=%.o))

$(BUILD_FOLDER)/%.o: $(SRC_FOLDER)/%.c
	$(CC) $(CFLAGS) -I $(SRC_FOLDER) -c -o $@ $<


build: prepare $(OBJECTS)
	$(info    SRC is $(SRC))
	$(info    OBJECTS is $(OBJECTS))
	$(CC) $(CFLAGS) -I $(SRC_FOLDER) -o $(OUT) $(OBJECTS)
	du -b $(OUT)
	avr-size -C --mcu=atmega328p $(OUT)


#build: prepare
#	$(CC) $(CFLAGS) -o $(OUT) $(SRC)




flash: prepare build
	$(PG) $(PFLAGS) -P $(USB) -U flash\:w\:$(OUT)

clean:
	@echo ">removing build"; \
	rm -rf $(BUILD_FOLDER)/*

prepare:
	@if [ ! -d "build" ]; then \
		echo ">creating build folder"; \
		mkdir $(BUILD_FOLDER); \
	else \
		echo ">build folder exists"; \
	fi
