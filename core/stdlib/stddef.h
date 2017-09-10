#pragma once

#define NULL (void*)0
typedef unsigned long size_t;
typedef signed long ptrdiff_t;

#define offsetof(st, m) ((size_t)&(((st *)0)->m))
