#include <stdlib.h>
#include <string.h>

#include "screen.h"

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

int arg_stub() {
	write_str("get arg\n");
	__halt();
	return 0;
}

size_t heap_base;
size_t heap_top;

void setup_heap(size_t base, size_t top) {
	heap_base = base;
	heap_top = top;
}

void* realloc(void *ptr, size_t size) {
	if (size == 0) {
		write_str("alloc size = 0\n");
		__halt();
	}
	
	void* new_ptr = heap_base;
	heap_base += size;

	if (ptr != NULL) {
		memcpy(new_ptr, ptr, size);
		free(ptr);
	}

	return new_ptr;
}

void free(void *ptr) {
	// TODO
}

unsigned long int strtoul(const char* str, char** endptr, int base) {
	write_str("str2ul\n");
	__halt();
	return -1;
}