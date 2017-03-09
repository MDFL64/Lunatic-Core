#include "screen.h"

// String

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
	const char* _ptr1 = ptr1;
	const char* _ptr2 = ptr2;

	while (*_ptr1 == *_ptr2 && num > 0) {
		_ptr1++;
		_ptr2++;
		num--;
	}

	if (num == 0)
		return 0;

	return *_ptr1 - *_ptr2;
}

void* memcpy(void* dest, const void* src, size_t count) {
	unsigned char* ptr_dest = dest;
	const unsigned char* ptr_src = src;

	while (count > 0) {
		*ptr_dest = *ptr_src;
		ptr_dest++;
		ptr_src++;
		count--;
	}

	return dest;
}

/*void* memset(void* dest, int c, size_t count) {
	unsigned char* ptr = dest;

	while (count > 0) {
		*ptr = c;
		ptr++;
		count--;
	}
}*/

void * memmove(void * destination, const void * source, size_t num) {
	write_str_halt("memmove\n");
	return 0;
}

void *memchr(const void *s, int c, size_t n) {
	write_str_halt("memchr\n");
	return 0;
}

char *strncpy(char * destination, const char * source, size_t num) {
	write_str_halt("strncpy\n");
	return 0;
}

char * strpbrk(const char * str1, const char * str2) {
	write_str_halt("[STRPBRK]\n");
	return NULL;
}

char *strstr(const char *haystack, const char *needle) {
	write_str_halt("[strstr]\n");
	return 0;
}

// Pretty sure this works.
char* strchr(const char* str, int character) {
	//write_str("strchr\n");
	//write_str(str);
	//__halt();
	
	while (*str != 0 && *str != character)
		str++;

	if (*str == 0)
		return NULL;

	return (char*)str;
}

char *strerror(int errnum) {
	write_str_halt("strerror\n");
	return 0;
}