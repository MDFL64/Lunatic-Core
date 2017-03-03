#pragma once

int memcmp(const void * ptr1, const void * ptr2, size_t num);
void* memcpy(void* dest, const void* src, size_t count);
void* memset(void* dest, int c, size_t count);
#pragma intrinsic(memcmp,memcpy,memset)

int strcmp(const char * str1, const char * str2);
char * strcpy(char * destination, const char * source);
size_t strlen(const char * str);
#pragma intrinsic(strcmp,strcpy,strlen)

const char* strchr(const char* str, int character);

double strtod(const char* str, char** endptr);

const char * strpbrk(const char * str1, const char * str2);

int strcoll(const char * str1, const char * str2);

size_t strspn(const char * str1, const char * str2);