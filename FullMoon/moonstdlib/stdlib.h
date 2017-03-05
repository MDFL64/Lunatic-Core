#pragma once // DONE

#define EXIT_FAILURE -1

void exit(int status);

void *realloc(void *ptr, size_t size);
void free(void *ptr);

unsigned long int strtoul(const char* str, char** endptr, int base);