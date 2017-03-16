#include <screen.h>

void _assert_f(int c, const char* msg) {
	if (!c) write_str_halt(msg);
}