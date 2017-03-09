#include "screen.h"

// String

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
	write_str("memcmp\n");
	__halt();
	
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

/*void* memcpy(void* dest, const void* src, size_t count) {
	unsigned __int8* ptr_dest = dest;
	const unsigned __int8* ptr_src = src;

	while (count > 0) {
		*ptr_dest = *ptr_src;
		ptr_dest++;
		ptr_src++;
		count--;
	}
}

void* memset(void* dest, int c, size_t count) {
	unsigned __int8* ptr = dest;

	while (count > 0) {
		*ptr = c;
		ptr++;
		count--;
	}
}*/

void * memmove(void * destination, const void * source, size_t num) {
	write_str("memmove\n");
	__halt();
	return 0;
}

void *memchr(const void *s, int c, size_t n) {
	write_str("memchr\n");
	__halt();
	return 0;
}

char *strncpy(char * destination, const char * source, size_t num) {
	write_str("strncpy\n");
	__halt();
	return 0;
}

const char * strpbrk(const char * str1, const char * str2) {
	write_str("[STRPBRK]\n");
	__halt();
	return str1;
}

char *strstr(const char *haystack, const char *needle) {
	write_str("[strstr]\n");
	__halt();
	return 0;
}

// Pretty sure this works.
const char* strchr(const char* str, int character) {
	//write_str("strchr\n");
	//write_str(str);
	//__halt();
	
	while (*str != 0 && *str != character)
		str++;

	if (*str == 0)
		return NULL;

	return str;
}

char *strerror(int errnum) {
	write_str("strerror\n");
	return 0;
}