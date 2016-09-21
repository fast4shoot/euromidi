#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

void display_setup();
void display_show(uint8_t a, uint8_t b, uint8_t c);
void display_leds(uint8_t leds);
void display_show_note(uint8_t note);
void display_show_number(uint8_t number);

#endif
