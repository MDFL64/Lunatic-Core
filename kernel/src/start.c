#include <stdlib.h>

#include <lauxlib.h>
#include <lualib.h>

#include "screen.h"

struct {
	size_t memory_top;
	size_t kernel_top;
	unsigned int init_file_count;
}* kernel_info_table = (void*)0x60000;

void start() {
	//write_str("Balls. ");
	//write_int(1337);
	//write_char(' ');
	//write_int(6969);

	/*write_int(strlen(""));
	write_char('\n');
	write_int(strlen("xyzzy"));
	write_char('\n');
	write_int(strlen("The quick brown fox jumps over the lazy MEME!"));
	write_char('\n');*/

	setup_heap(kernel_info_table->kernel_top, kernel_info_table->memory_top);
	
	write_str("HELLO!\n");

	lua_State* state = luaL_newstate();
	luaL_openlibs(state);

	const char* test = "local x = 0 for i=1,1000 do x=x+i end print('please') return x";

	write_str("Test script: ");
	write_str(test);
	write_char('\n');

	if (luaL_loadstring(state, test)) {
		write_str_halt("Failed to load test script.");
	}

	lua_call(state, 0, 1);
	double res = lua_tonumber(state, -1);
	
	write_str("Result: ");
	write_int(res);
	write_str("\n");
	
	write_str_halt("ALL GOOD IN THE HOOD!\n");
}