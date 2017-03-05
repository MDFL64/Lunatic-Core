#include <stdio.h>

#include "../src/screen.h"

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
	write_str("fwrite\n");
	__halt();
	return -1;
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
	write_str("putchar\n");
	__halt();
	return -1;
}