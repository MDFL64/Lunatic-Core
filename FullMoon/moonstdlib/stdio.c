#include <stdio.h>

#include "screen.h"

FILE* stdin = 1;
FILE* stdout = 2;
FILE* stderr = 3;

FILE * fopen(const char * filename, const char * mode) {
	write_str("fopen\n");
	__halt();
	return -1;
}

int fclose(FILE * stream) {
	write_str("fclose\n");
	__halt();
	return -1;
}

size_t fread(void * ptr, size_t size, size_t count, FILE * stream) {
	write_str("fread\n");
	__halt();
	return -1;
}

size_t fwrite(const void * ptr, size_t size, size_t count, FILE * stream) {
	if (stream == stdout) {
		write_str_n(ptr, size * count);
		write_str("\n");
		return count;
	}
	else {
		write_str("fwrite to: ");
		write_int(stream);
		write_str("\n");
		__halt();
	}
}

int sprintf(char * str, const char * format, ...) {
	write_str("sprintf\n");
	__halt();
	return -1;
}

int fputc(int character, FILE * stream) {
	write_str("fputc\n");
	__halt();
	return -1;
}

int fputs(const char * str, FILE * stream) {
	write_str("fputs\n");
	write_str(str);
	__halt();
	return -1;
}

int fflush(FILE * stream) {
	write_str("fflush\n");
	__halt();
	return -1;
}

int feof(FILE * stream) {
	write_str("feof\n");
	__halt();
	return -1;
}

int ferror(FILE * stream) {
	write_str("fputc\n");
	__halt();
	return -1;
}

int putchar(int character) {
	write_char(character);
	return character;
}