#pragma once

#include <stddef.h>

#define EXIT_SUCCESS 1
#define EXIT_FAILURE 2

void exit(int status);

void setup_heap(size_t base, size_t top);

void *realloc(void *ptr, size_t size);
void free(void *ptr);

unsigned long int strtoul(const char* str, char** endptr, int base);

// NEW

int system(const char* command);

char* getenv(const char* name);