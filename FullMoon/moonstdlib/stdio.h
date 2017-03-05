#pragma once // DONE

typedef size_t FILE;

#define BUFSIZ 1024

FILE* stdin;
FILE* stdout;
FILE* stderr;

FILE * fopen(const char * filename, const char * mode);
int fclose(FILE * stream);

size_t fread(void * ptr, size_t size, size_t count, FILE * stream);
size_t fwrite(const void * ptr, size_t size, size_t count, FILE * stream);

int sprintf(char * str, const char * format, ...);

int fputc(int character, FILE * stream);
int fputs(const char * str, FILE * stream);

int fflush(FILE * stream);

int feof(FILE * stream);
int ferror(FILE * stream);

int putchar(int character);