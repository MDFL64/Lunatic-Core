#include <string.h>

#ifdef LUNATIC_USERMODE
#include <stdio.h>
#endif

#include "screen.h"

// Hopefully this is the actual terminal size.
const int TERM_W = 80;
const int TERM_H = 25;

int cursor_i = 0;
char* video_memory = (char*)0xB8000;

const char blankline[] = { 0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7 };

void scroll_down() {
	for (int i = 0; i<TERM_H - 1; i++)
		memcpy(video_memory + i * TERM_W * 2, video_memory + (i + 1) * TERM_W * 2, TERM_W * 2);
	memcpy(video_memory + (TERM_H - 1) * TERM_W * 2, blankline, TERM_W * 2);
}

#ifdef LUNATIC_USERMODE
#define update_cursor()
#else
void update_cursor() {
	asm(
		"movw $0x03D4, %%dx\n"
		"movb $0x0F, %%al\n"
		"outb %%al, %%dx\n"

		"movw $0x03D5, %%dx\n"
		"movl %0, %%eax\n"
		"outb %%al, %%dx\n"

		"movw $0x03D4, %%dx\n"
		"movb $0x0E, %%al\n"
		"outb %%al, %%dx\n"

		"movw $0x03D5, %%dx\n"
		"movl %1, %%eax\n"
		"outb %%al, %%dx\n"
		:
		: "r"(cursor_i), "r"(cursor_i >> 8)
		: "dx","eax"
	);
}
#endif

void write_c(char c) {
	#ifdef LUNATIC_USERMODE
	putchar(c);
	#else
	if (c == '\n') {
		cursor_i = ((cursor_i / TERM_W) + 1) * TERM_W;
	}
	else if (c == '\t') {
		cursor_i += 4;
	}
	else {
		video_memory[cursor_i * 2] = c;
		cursor_i++;
	}

	if (cursor_i >= TERM_W*TERM_H) {
		scroll_down();
		cursor_i = (TERM_H - 1) * TERM_W;
	}
	#endif
}

void write_char(char c) {
	write_c(c);
	update_cursor();
}

void write_str(const char* str) {
	if (str == NULL)
		return;

	int i = 0;
	while (str[i])
		write_c(str[i++]);
	update_cursor();
}

void write_str_halt(const char* str) {
	write_str(str);
	#ifdef LUNATIC_USERMODE
	exit(1);
	#else
	asm("hlt");
	#endif
}

void write_str_n(const char* str, size_t n) {
	if (str == NULL)
		return;

	int i = 0;
	while (i < n)
		write_c(str[i++]);
	update_cursor();
}

void write_int(long n) {
	if (n<0) {
		write_c('-');
		n = -n;
	}

	long place = 1000000000000000000;

	while (place > n && place > 1) {
		place /= 10;
	}

	while (place > 0) {
		int d = n / place;
		write_c('0' + d);
		n -= d*place;

		place /= 10;
	}
	update_cursor();
}

void write_hex(unsigned long n) {
	unsigned long mask = 0xF000000000000000;
	int shift = 60;

	write_c('0');
	write_c('x');

	while ((mask & n) == 0 && shift>0) {
		mask >>= 4;
		shift -= 4;
	}

	while (shift>=0) {
		int d = (mask & n) >> shift;
		if (d<10)
			write_c('0'+d);
		else {
			write_c('A' + d - 10);
		}
		mask >>= 4;
		shift -= 4;
	}
	update_cursor();
}