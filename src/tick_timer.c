#include "tick_timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include "events.h"

void tick_timer_setup() {
	TCCR0A = 0x0;
	TCCR0B = (1<<CS02); // divide clock by 64 to get an interrupt approximately every millisecond
	TIMSK0 = (1<<TOIE0); // enable overflow interrupts
}

ISR(TIMER0_OVF_vect) {
	event_push((event_t){.id = event_tick});
}
