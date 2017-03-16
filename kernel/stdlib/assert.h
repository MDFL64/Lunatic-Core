#pragma once

#include <screen.h>

void _assert_f(int c, const char* msg);

// Thanks SO.
// http://stackoverflow.com/questions/5459868/c-preprocessor-concatenate-int-to-string
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define assert(x) _assert_f(x,"Assertion failed: " __FILE__ " > "STR(__LINE__))