#include <stdlib.h>

#include <lj_obj.h>

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

void hook_func(lua_State *state, lua_Debug *dbg) {
	write_str("~hook~\n");
	lua_yield(state,0); // please
	write_str("~meme~\n");
}

void start() {

	setup_heap(kernel_info_table->kernel_top, kernel_info_table->memory_top);
	
	write_str(" - FULLMOON KERNEL LOADED -\n");

	lua_State* state1 = luaL_newstate();
	luaL_openlibs(state1);

	lua_State* state2 = luaL_newstate();
	luaL_openlibs(state2);

	//lua_State *thread1 = lua_newthread(state1);


	const char* code1 = "while true do print('Thread 1 here!') end";
	const char* code2 = "while true do print('Thread 2 here!') end";

	if (luaL_loadstring(state1, code1)) {
		write_str_halt("Failed to load test script 1.");
	}

	if (luaL_loadstring(state2, code2)) {
		write_str_halt("Failed to load test script 2.");
	}

	lua_sethook(state1, hook_func, LUA_MASKCOUNT, 100);

	int res;

	for (;;) {
		write_str("resuming..\n");
		res = lua_resume(state1, 0);
		if (res!=LUA_YIELD)
			write_str_halt("unexpected result!");
		write_str("juan ");
		write_int(lua_gettop(state1));
		asm("hlt");
		
		/*res = lua_resume(state2, 0);
		if (res!=LUA_YIELD)
			write_str_halt("unexpected result!");*/
	}


	write_str_halt("All done. Halting.\n");
}