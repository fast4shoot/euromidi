#include "events.h"

static event_t event_queue[EVENT_QUEUE_SIZE];
static volatile uint8_t event_queue_begin = 0;
static volatile uint8_t event_queue_end = 0;

void event_push(event_t event) {
	// this may overflow the queue, we just have to hope it doesn't
	uint8_t end = event_queue_end;
	event_queue[end] = event;
	event_queue_end = (end + 1) % EVENT_QUEUE_SIZE;
}

bool event_peek(event_t* event) {
	uint8_t begin = event_queue_begin;
	if (begin == event_queue_end) {
		return false;
	} else {
		*event = event_queue[begin];
		event_queue_begin = (begin + 1) % EVENT_QUEUE_SIZE;
		return true;
	}
}
