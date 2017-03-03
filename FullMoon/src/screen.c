#include <string.h>

// Hopefully this is the actual terminal size.
const int TERM_W = 80;
const int TERM_H = 25;

int cursor_i = 0;
__int8* video_memory = 0xB8000;

const char blankline[] = { 0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7 };

void scroll_down() {
	for (int i = 0; i<TERM_H - 1; i++)
		memcpy(video_memory + i * TERM_W * 2, video_memory + (i + 1) * TERM_W * 2, TERM_W * 2);
	memcpy(video_memory + (TERM_H - 1) * TERM_W * 2, blankline, TERM_W * 2);
}

void write_char(char c) {
	if (c == '\n') {
		cursor_i = ((cursor_i / TERM_W) + 1) * TERM_W;
	}
	else {
		video_memory[cursor_i * 2] = c;
		cursor_i++;
	}

	if (cursor_i >= TERM_W*TERM_H) {
		scroll_down();
		cursor_i = (TERM_H - 1) * TERM_W;
	}
}

void update_cursor() {
	__outbyte(0x03D4, 0x0F);
	__outbyte(0x03D5, cursor_i & 0xFF);
	__outbyte(0x03D4, 0x0E);
	__outbyte(0x03D5, (cursor_i >> 8) & 0xFF);
}

void write_str(const char* str) {
	if (str == NULL)
		return;

	int i = 0;
	while (str[i])
		write_char(str[i++]);
	update_cursor();
}

void write_int(__int64 n) {
	if (n<0) {
		write_char('-');
		n = -n;
	}

	__int64 place = 1000000000000000000;

	while (place > n && place > 1) {
		place /= 10;
	}

	while (place > 0) {
		int d = n / place;
		write_char('0' + d);
		n -= d*place;

		place /= 10;
	}
}