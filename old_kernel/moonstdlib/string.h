#pragma once // DONE

int strcmp(const char * str1, const char * str2);
char * strcpy(char * destination, const char * source);
size_t strlen(const char * str);
#pragma intrinsic(strcmp,strcpy,strlen)

int memcmp(const void * ptr1, const void * ptr2, size_t num);
void* memcpy(void* dest, const void* src, size_t count);
void* memset(void* dest, int c, size_t count);
#pragma intrinsic(memcmp,memcpy,memset)

void * memmove(void * destination, const void * source, size_t num);
void *memchr(const void *s, int c, size_t n);

char * strncpy(char * destination, const char * source, size_t num);

const char * strpbrk(const char * str1, const char * str2);

char *strstr(const char *haystack, const char *needle);
const char* strchr(const char* str, int character);

char *strerror(int errnum);