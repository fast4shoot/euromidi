PROG = main
MCU2 = m328p

SOURCES = main.c i2c.c spi.c midi.c events.c display.c font.c

default:
	avr-gcc @config -o $(PROG).elf $(addprefix src/,$(SOURCES))

flash: default
	avrdude -p $(MCU2) -c usbasp-clone -U flash:w:$(PROG).elf:e
