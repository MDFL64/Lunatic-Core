#pragma once

typedef size_t jmp_buf;

void longjmp(jmp_buf env, int val);

int setjmp(jmp_buf env);