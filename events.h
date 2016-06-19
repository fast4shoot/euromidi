#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>
#include <stdbool.h>

// Enum of all possible events.
// Some events may have exactly two bytes of additional data.
typedef enum {
	// midi channel mode events
	// these have additional data
	event_note_off = 0x80,
	event_note_on = 0x90,
	event_control_change = 0xb0,
	event_pitch_bend = 0xe0,

	// midi realtime events
	event_clock = 0xf8,
	event_start = 0xfa,
	event_continue = 0xfb,
	event_stop = 0xfc,
	
	// time events
	event_tick = 0x0,

	// encoder events
	event_encoder_up = 0x1,
	event_encoder_down = 0x2,
	event_encoder_press = 0x3
} event_id_t;

// We store each event in this struct to make
// everything easier.
typedef struct {
	event_id_t event_id;
	uint8_t a, b;
} event_t;

void event_push(event_t event);
bool event_peek(event_t* event);

#endif
