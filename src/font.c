#include "font.h"

#define A 0x01
#define B 0x02
#define C 0x40
#define D 0x04
#define E 0x10
#define F 0x20
#define G 0x08

const uint8_t font[128] = {
	[' '] = 0,

	// digits
	['0'] = 0x77,
	['1'] = 0x42,
	['2'] = 0x1f,
	['3'] = 0x4f,
	['4'] = 0x6a,
	['5'] = 0x6d,
	['6'] = 0x7d,
	['7'] = 0x43,
	['8'] = 0x7f,
	['9'] = 0x6f,

	// letters
	['A'] = A|B|C|E|F|G,
	['B'] = C|D|E|F|G,
	['C'] = A|D|E|F,
	['D'] = B|C|D|E|G,
	['E'] = A|D|E|F|G,
	['F'] = A|E|F|G,
	['G'] = A|C|D|E|F,
	['H'] = B|C|E|F|G,
	['I'] = B|C, // the same as a one
	['J'] = B|C|D|E,
	// K 
	['L'] = D|E|F,
	// M
	// N
	['O'] = A|B|C|D|E|F, // the same as a zero
	['P'] = A|B|E|F|G,
	// Q
	// R
	['S'] = A|F|G|D|C, // the same as a five
	// T
	['U'] = B|C|D|E|F,
	// V
	// W
	// X
	['Y'] = B|C|D|F|G,
    ['Z'] = A|B|D|E|G, // the same as a two

	['-'] = G,
	['_'] = D,
	['#'] = C|D|E|G, // eh, close enough
};
