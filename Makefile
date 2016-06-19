PROG = main
MCU2 = m328p

default:
	avr-gcc @config -o $(PROG).elf $(PROG).c i2c.c spi.c midi.c events.c

flash: default
	avrdude -p $(MCU2) -c usbasp-clone -U flash:w:$(PROG).elf:e
