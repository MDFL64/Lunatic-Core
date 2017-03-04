#include "screen.h"

#include <stdio.h>
#include <locale.h>

#include <setjmp.h>
#include <time.h>

// STD

void abort() {
	// ?
	write_str("[ABORT]\n");
	__halt();
}

void _fltused() {
	write_str("[FLOAT]\n");
	__halt();
}

void __chkstk() {
	write_str("[STACK]\n");
	__halt();
}

// IO

FILE * fopen(const char * filename, const char * mode) {
	write_str("[FOPEN]\n");
	__halt();
	return 0;
}

int fclose(FILE * stream) {
	write_str("[FCLOSE]\n");
	__halt();
	return 0;
}

int fflush(FILE * stream) {
	write_str("[FFLUSh]\n");
	__halt();
	return 0;
}

int fprintf(FILE * stream, const char * format, ...) {
	write_str("[FPRINTF]\n");
	__halt();
	return 0;
}

size_t fread(void * ptr, size_t size, size_t count, FILE * stream) {
	write_str("[FREAD]\n");
	__halt();
	return 0;
}

int n = 0;

int snprintf(char * s, size_t n, const char * format, ...) {
	write_str("> called snprintf: ");
	write_str(format);
	write_str("\n");
	s[0] = '?';
	s[1] = 0;
	n++;
	//if (n>20)
	//	__halt();
	return 1;
}

// Locale

lconv locale = {
	.decimal_point = "."
};

lconv* localeconv() {
	write_str("[LOCALE]\n");
	__halt();
	return &locale;
}

// Time

time_t time(time_t* timer) {
	write_str("> called time\n");
	return 1234;
}

// String

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
	const unsigned __int8* _ptr1 = ptr1;
	const unsigned __int8* _ptr2 = ptr2;

	while (*_ptr1 == *_ptr2 && num > 0) {
		_ptr1++;
		_ptr2++;
		num--;
	}

	if (num == 0)
		return 0;

	return *_ptr1 - *_ptr2;
}

void* memcpy(void* dest, const void* src, size_t num) {
	unsigned __int8* _dest = dest;
	const unsigned __int8* _src = src;
	
	while (num > 0) {
		*_dest = *_src;

		_dest++;
		_src++;
		num--;
	}

	return dest;
}

const char* strchr(const char* str, int character) {
	while (*str != 0 && *str != character)
		str++;

	if (*str == 0)
		return NULL;

	return str;
}

double strtod(const char* str, char** endptr) {
	write_str("[STR2D]\n");
	__halt();
	return 0;
}

const char * strpbrk(const char * str1, const char * str2) {
	write_str("[STRPBRK]\n");
	__halt();
	return str1;
}

int strcoll(const char * str1, const char * str2) {
	write_str("[STRCOLL]\n");
	__halt();
	return 0;
}

size_t strspn(const char * str1, const char * str2) {
	write_str("[STRSPN]\n");
	__halt();
	return 0;
}

// Math
double abs(double x) {
	if (x > 0)
		return x;
	else
		return -x;
}

double pow(double x) {
	write_str("[POW]\n");
	__halt();
	return -1;
}

double fmod(double x) {
	write_str("[FMOD]\n");
	__halt();
	return -1;
}

double frexp(double x, int* exp) {
	write_str("[FREXP]\n");
	__halt();
	return -1;
}

double floor(double x) {
	__int64 trun = x;
	if (x<0 && trun != x)
		return (__int64)(x - 1);
	return (__int64)(x);
}

// long jump
void longjmp(jmp_buf env, int val) {
	write_str("Long jump attempted, halting.\n");
	__halt();
}

int setjmp(jmp_buf env) {
	write_str("> called setjmp\n");
	//unsigned __int64 state = 3;
	//_xsave64(&env, state);
	return 0;
}