#pragma once // DONE

#define NULL (void*)0

typedef unsigned __int64 size_t;

// LUAJIT GARBAGE:
#define _MSC_VER

#define LUAJIT_TARGET	LUAJIT_ARCH_X64

#define LUAJIT_OS	LUAJIT_OS_OTHER

#define LUAJIT_DISABLE_JIT
#define LUAJIT_DISABLE_FFI

#define LUAJIT_USE_SYSMALLOC