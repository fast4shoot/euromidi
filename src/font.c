#include "font.h"

#define A 0x40
#define B 0x20
#define C 0x10
#define D 0x08
#define E 0x04
#define F 0x02
#define G 0x01

const uint8_t font[128] = {
	[' '] = 0,

	// digits
	['0'] = A|B|C|D|E|F,
	['1'] = B|C,
	['2'] = A|B|D|E|G,
	['3'] = A|B|C|D|G,
	['4'] = B|C|F|G,
	['5'] = A|C|D|F|G,
	['6'] = A|C|D|E|F|G,
	['7'] = A|B|C,
	['8'] = A|B|C|D|E|F|G,
	['9'] = A|B|C|D|F|G,

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
