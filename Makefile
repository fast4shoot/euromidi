PROG = main
MCU2 = m328p

SOURCES = display.c encoder.c events.c font.c i2c.c main.c midi.c spi.c tick_timer.c ui.c

default:
	avr-gcc @config -o $(PROG).elf $(addprefix src/,$(SOURCES))

flash: default
	avrdude -p $(MCU2) -c usbasp-clone -U flash:w:$(PROG).elf:e
