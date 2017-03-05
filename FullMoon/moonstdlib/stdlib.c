#include "../src/screen.h"

void _fltused() {
	write_str("[FLOAT]\n");
	__halt();
}

void __chkstk() {
	write_str("[STACK]\n");
	__halt();
}

unsigned long _byteswap_ulong(unsigned long x) {
	write_str("[byteswap]\n");
	__halt();
}

void exit(int status) {
	write_str("exit\n");
	__halt();
	return -1;
}

void *realloc(void *ptr, size_t size) {
	write_str("alloc\n");
	__halt();
	return -1;
}

void free(void *ptr) {
	write_str("free\n");
	__halt();
	return -1;
}

unsigned long int strtoul(const char* str, char** endptr, int base) {
	write_str("str2ul\n");
	__halt();
	return -1;
}