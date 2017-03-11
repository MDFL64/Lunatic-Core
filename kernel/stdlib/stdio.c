#include <stdio.h>

#include "screen.h"

FILE* stdin = (FILE*)-1;
FILE* stdout = (FILE*)-2;
FILE* stderr = (FILE*)-3;

FILE* fopen(const char * filename, const char * mode) {
	write_str_halt("fopen\n");
	return NULL;
}

int fclose(FILE* stream) {
	write_str_halt("fclose\n");
	return -1;
}

size_t fread(void * ptr, size_t size, size_t count, FILE* stream) {
	write_str_halt("fread\n");
	return -1;
}

size_t fwrite(const void * ptr, size_t size, size_t count, FILE* stream) {
	if (stream == stdout) {
		write_str_n(ptr, size * count);
		return count;
	}
	else {
		write_str("fwrite to: ");
		write_int((long)stream);
		write_str("\n");
		write_str_halt(":(");
		return 0;
	}
}

int sprintf(char * str, const char * format, ...) {
	write_str_halt("sprintf\n");
	return -1;
}

int fputc(int character, FILE * stream) {
	write_str_halt("fputc\n");
	return -1;
}

int fputs(const char * str, FILE * stream) {
	write_str_halt("fputs\n");
	write_str(str);
	return -1;
}

int fflush(FILE * stream) {
	write_str_halt("fflush\n");
	return -1;
}

int feof(FILE * stream) {
	write_str_halt("feof\n");
	return -1;
}

int ferror(FILE * stream) {
	write_str_halt("fputc\n");
	return -1;
}

int putchar(int character) {
	write_char(character);
	return character;
}

// NEW

int fseek(FILE* stream, long int offset, int whence) {
	write_str_halt("fseek\n");
	return -1;
}

long int ftell(FILE* stream) {
	write_str_halt("ftell\n");
	return -1;
}

int setvbuf(FILE* stream, char* buf, int type, size_t size) {
	write_str_halt("setvbuf\n");
	return -1;
}

void clearerr(FILE* stream) {
	write_str_halt("clearerr\n");
}

char* fgets(char* s, int n, FILE* stream) {
	write_str_halt("fgets\n");
	return NULL;
}

int fscanf(FILE* stream, const char* format, ... ) {
	write_str_halt("fscanf\n");
	return -1;
}

int getc(FILE* stream) {
	write_str_halt("getc\n");
	return -1;
}

int ungetc(int c, FILE* stream) {
	write_str_halt("ungetc\n");
	return -1;
}

FILE* tmpfile () {
	write_str_halt("tmpfile\n");
	return NULL;
}

int remove(const char* path) {
	write_str_halt("remove\n");
	return -1;
}

int rename(const char* old, const char* new) {
	write_str_halt("rename\n");
	return -1;
}

char* tmpnam(char* s) {
	write_str_halt("tmpnam\n");
	return NULL;
}