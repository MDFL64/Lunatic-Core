#pragma once

#include "screen.h"

typedef __builtin_va_list va_list;

#define va_arg(args,type) __builtin_va_arg(args,type)

#define va_start(args,last) __builtin_va_start(args,last)

#define va_end(args) __builtin_va_end(args)