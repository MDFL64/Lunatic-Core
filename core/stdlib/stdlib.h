#pragma once

#include <stddef.h>

#define EXIT_FAILURE 1

void exit(int status);

void setup_heap(size_t base, size_t top);
void check_heap();

void *realloc(void *ptr, size_t size);
void free(void *ptr);

unsigned long int strtoul(const char* str, char** endptr, int base);