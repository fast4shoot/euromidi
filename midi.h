#ifndef MIDI_RECV_H
#define MIDI_RECV_H

#include <stdint.h>

/// Inicializuje usart pro prijem midi
void midi_recv_init();

/// Vrati pocet bajtu ve vstupnim bufferu.
uint8_t midi_recv_get_count();

/// Vrati byte ze vstupniho bufferu pokud tam nejaky je,
/// jinak vrati nedefinovanou hodnotu.
uint8_t midi_recv_get_byte();

#endif
