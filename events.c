#include "events.h"

static event_t event_queue[EVENT_QUEUE_SIZE];
static uint8_t event_queue_begin = 0;
static uint8_t event_queue_end = 0;

void event_push(event_t event) {
	// this may overflow the queue, we just have to hope it doesn't
	event_queue[event_queue_end] = event;
	event_queue_end = (event_queue_end + 1) % EVENT_QUEUE_SIZE;
}

bool event_peek(event_t* event) {
	if (event_queue_begin == event_queue_end) {
		return false;
	} else {
		*event = event_queue[event_queue_begin];
		event_queue_begin = (event_queue_begin + 1) % EVENT_QUEUE_SIZE;
		return true;
	}
}
