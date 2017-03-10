#pragma once

#include <stddef.h>

typedef size_t FILE;

#define L_tmpnam 100

#define BUFSIZ 1024

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

#define _IOFBF 1
#define _IOLBF 2
#define _IONBF 3

#define EOF -1

FILE * fopen(const char * filename, const char * mode);
int fclose(FILE * stream);

size_t fread(void * ptr, size_t size, size_t count, FILE * stream);
size_t fwrite(const void * ptr, size_t size, size_t count, FILE * stream);

int sprintf(char * str, const char * format, ...);

int fputc(int character, FILE* stream);
int fputs(const char* str, FILE* stream);

int fflush(FILE * stream);

int feof(FILE* stream);
int ferror(FILE* stream);

int putchar(int character);

// NEW

int fseek(FILE* stream, long int offset, int whence);
long int ftell(FILE* stream);

int setvbuf(FILE* stream, char* buf, int type, size_t size);

void clearerr(FILE* stream);

char* fgets(char* s, int n, FILE* stream);

int fscanf(FILE* stream, const char* format, ... );

int getc(FILE* stream);

int ungetc(int c, FILE* stream);

FILE* tmpfile();

int remove(const char* path);

int rename(const char* old, const char* new);

char* tmpnam(char* s);