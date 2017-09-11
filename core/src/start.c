#include <stdlib.h>

#include <lj_obj.h>
#include <lj_frame.h>

#include <lauxlib.h>
#include <lualib.h>

#include "screen.h"

#pragma pack(push,1)
struct {
	size_t memory_top;
	size_t kernel_top;
	unsigned int init_file_count;
	struct {char name [11]; int addr; int size} init_files [];
}* kernel_info_table = (void*)0x60000;
#pragma pack(pop)

void start_debug() {
	asm(
		// set addr
		"movq $0x103ce88, %%rax\n"
		"movq %%rax, %%dr0\n"

		// global enable cr0, global enable, bit 11, ICE
		"movq $0x1602, %%rax\n"
		"movq %%rax, %%dr7\n"
		:
		:
		: "rax"
	);
}

void check_frames(lua_State* state) {
	cTValue *frame = state->base-1;
	cTValue *bot = tvref(state->stack)+LJ_FR2;
	
	while (frame > bot) {
		write_str("Frame: ");
		write_hex(frame);
		write_char(' ');
		write_int(frame_pc(frame));
		write_char(' ');
		write_int(frame_typep(frame));

		if (frame_typep(frame) == FRAME_CONT) {
			write_str(" CONTINUATION ");
			write_hex(frame_contpc(frame));
			write_char(' ');
			write_hex(frame_contv(frame));
		}

		write_char('\n');

		frame = frame_prev(frame);
	}
}

void hook_func(lua_State *state, lua_Debug *dbg) {
	lua_yield(state,0); // please
}

/*void ballus(int n, ...) {
	va_list vargs;
	va_start(vargs, n);
	for (int i=0;i<n;i++) {
		int x = va_arg(vargs,int);
			write_hex(x);
			write_str("\n");
	}
	va_end(vargs);
}*/

char* start() {
	int heap_bottom = kernel_info_table->kernel_top;
	int heap_top = kernel_info_table->memory_top;
	if (heap_top>1073741824)
		heap_top = 1073741824;

	setup_heap(heap_bottom, heap_top);

	char* init_addr = 0;
	size_t init_size = 0;
	for (int i=0;i<kernel_info_table->init_file_count;i++) {
		if (memcmp("INIT    LUA",kernel_info_table->init_files[i].name,11) == 0) {
			init_addr = (char*)(kernel_info_table->init_files[i].addr);
			init_size = kernel_info_table->init_files[i].size;
			break;
		}
	}

	if (init_addr==0) {
		return "Could not find kernel.";
	}

	lua_State* state = luaL_newstate();
	luaL_openlibs(state);
	luaopen_ffi(state);
	lua_setglobal(state,"ffi");

	if (luaL_loadbuffer(state, init_addr, init_size, "kernel.lua")) {
		return "Error loading init: ???";
	}

	if (lua_pcall(state,0,0,0) != 0) {
		write_str("Error in init:");
		return lua_tostring(state,-1);
	}

	return "Clean exit.";

	int res;

	for (;;) {
		//write_str("resuming...\n");

		//res = lua_resume(state, 0);
		if (res!=LUA_YIELD) {
			write_str_halt(lua_tostring(state,-1));
			write_str_halt(" ~ Unexpected result!");
		}
		
		//res = lua_resume(state2, 0);
		if (res!=LUA_YIELD) {
			//write_str_halt(lua_tostring(state2,-1));
		}
		
		//write_str("OUT\n");
		//check_frames(state);

		//write_hex(state);
		//start_debug();
		
		//write_str_halt("Halt!");
		//write_int(lua_gettop(state));
		//asm("hlt");
		
	}


	//write_str_halt("All done. Halting.\n");
}