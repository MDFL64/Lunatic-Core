#pragma once // DONE

typedef long va_list;

int arg_stub();

#define va_arg(args,type) arg_stub()

#define va_start(args,last)

#define va_end(args)