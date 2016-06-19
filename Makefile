PROG = main
MCU2 = m328p

default:
	avr-gcc @config -Wall -Wextra -o $(PROG).elf $(PROG).c i2c.c spi.c
flash:
	avrdude -p $(MCU2) -c usbasp-clone -U flash:w:$(PROG).elf:e
