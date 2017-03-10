#pragma once

#include <stddef.h>

void *memchr(const void *s, int c, size_t n);
int memcmp(const void * ptr1, const void * ptr2, size_t num);
void* memcpy(void* dest, const void* src, size_t count);
void * memmove(void * destination, const void * source, size_t num);
void* memset(void* dest, int c, size_t count);

char* strchr(const char* str, int character);
int strcmp(const char * str1, const char * str2);
char * strcpy(char * destination, const char * source);
char *strerror(int errnum);
size_t strlen(const char * str);
char * strncpy(char * destination, const char * source, size_t num);
char * strpbrk(const char * str1, const char * str2);
char *strstr(const char *haystack, const char *needle);

// NEW

char *strrchr (const char * str, int character);