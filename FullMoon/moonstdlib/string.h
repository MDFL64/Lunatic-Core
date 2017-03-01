void* memcpy(void* dest, const void* src, size_t count);
#pragma intrinsic(memcpy)

void* memset(void* dest, int c, size_t count);
#pragma intrinsic(memset)

int strcmp(const char * str1, const char * str2);
#pragma intrinsic(strcmp)

size_t strlen(const char * str);
#pragma intrinsic(strlen)