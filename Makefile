PROG = main
MCU2 = m328p

default:
	avr-gcc @.syntastic_avrgcc_config  -Wall -O2 -o $(PROG).elf $(PROG).c i2c.c spi.c
	avrdude -p $(MCU2) -c usbasp-clone -U flash:w:$(PROG).elf:e
