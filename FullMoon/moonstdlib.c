#include "screen.h"

#include <stdio.h>
#include <locale.h>

#include <setjmp.h>
#include <time.h>

// STD

void abort() {
	// ?
}

void _fltused() {
	write_str("[FLOAT]\n");
}

void __chkstk() {
	write_str("[STACK]\n");
}

// IO

FILE * fopen(const char * filename, const char * mode) {
	return 0;
}

int fclose(FILE * stream) {
	return 0;
}

int fflush(FILE * stream) {
	return 0;
}

int fprintf(FILE * stream, const char * format, ...) {
	return 0;
}

size_t fread(void * ptr, size_t size, size_t count, FILE * stream) {
	return 0;
}

int snprintf(char * s, size_t n, const char * format, ...) {
	return 0;
}

// Locale

lconv locale = {
	.decimal_point = "."
};

lconv* localeconv() {
	return &locale;
}

// Time

time_t time(time_t* timer) {
	return 0;
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

	return str;
}

double strtod(const char* str, char** endptr) {
	return 0;
}

const char * strpbrk(const char * str1, const char * str2) {
	return str1;
}

int strcoll(const char * str1, const char * str2) {
	return 0;
}

size_t strspn(const char * str1, const char * str2) {
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
	return -1;
}

double fmod(double x) {
	return -1;
}

double frexp(double x, int* exp) {
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

}

int setjmp(jmp_buf env) {

}