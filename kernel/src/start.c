#include <stdlib.h>

#include <lauxlib.h>
#include <lualib.h>

#include "screen.h"

struct {
	size_t memory_top;
	size_t kernel_top;
	unsigned int init_file_count;
}* kernel_info_table = (void*)0x60000;

void start_debug() {
	asm(
		// set addr
		"movq $0x1027ed8, %%rax\n"
		"movq %%rax, %%dr0\n"

		// global enable cr0, global enable, bit 11, ICE
		"movq $0x1602, %%rax\n"
		"movq %%rax, %%dr7\n"
		:
		:
		: "rax"
	);
}

void start() {
	//asm("hlt");
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

	#ifndef LUNATIC_USERMODE
	setup_heap(kernel_info_table->kernel_top, kernel_info_table->memory_top);
	#endif
	
	write_str("FullMoon Kernel loaded!\n");

	lua_State* state = luaL_newstate();
	write_str("progress...\n");
	luaL_openlibs(state);

	const char* test = "for k,v in pairs(_G) do print(k,v) end return #_G";

	write_str("Test script: ");
	write_str(test);
	write_char('\n');

	if (luaL_loadstring(state, test)) {
		write_str_halt("Failed to load test script.");
	}

	//start_debug();

	lua_call(state, 0, 1);
	int res = lua_tonumber(state, -1);
	
	write_str("Result: ");
	
	#ifdef LUNATIC_USERMODE
	printf("%d",res);
	#else
	write_int(res);
	#endif

	write_str("\n");
	
	write_str_halt("All done. Halting.\n");
}

#ifdef LUNATIC_USERMODE
int main() {
	start();
	return 0;
}
#endif

// this is just an exception handler, 
void lj_err_unwind_win(void* rec,void* cf, void* ctx, void* dispatch) {
	write_str_halt("Windows exception handler called. WTF?\n");
}