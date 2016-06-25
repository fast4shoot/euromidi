#ifndef MIDI_SPEC_H
#define MIDI_SPEC_H

#define MIDI_IS_REALTIME(x) (((x) & 0xf8) == 0xf8)
#define MIDI_IS_STATUS(x) ((x) & 0x80)
#define MIDI_IS_CHANNEL_MODE(x) ((x) >= 0x80 && (x) < 0xf0)
#define MIDI_GET_CHANNEL(x) ((x) & 0xf)
#define MIDI_GET_MASKED_MSG(x) ((x) & 0xf0)

#define MIDI_NOTE_OFF 0x80
#define MIDI_NOTE_ON 0x90
#define MIDI_CONTROL_CHANGE 0xb0
#define MIDI_PITCH_BEND 0xe0

#endif
