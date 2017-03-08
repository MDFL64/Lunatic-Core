#include "screen.h"
#include <string.h>
#include <stdlib.h>
//#include <stdio.h>

#include "lauxlib.h"
#include "lualib.h"

struct {
	unsigned __int64 memory_top;
	unsigned __int64 kernel_top;
	unsigned __int32 init_file_count;
}* kernel_info_table = 0x60000;

size_t heap_base;
size_t heap_top;

/*void* moon_alloc(void* ud, void* optr, size_t osize, size_t nsize) {
	void* nptr = heap_base;
	heap_base += nsize;
	
	if (nsize == 0) {
		// Free!
		return NULL;
	}

	if (heap_base > heap_top) {
		write_str("CRITICAL: OUT OF MEMORY!");
		__halt();
	}

	if (optr != NULL)
		memcpy(new_ptr, optr, osize);

	//write_str("ALLOCATE ");
	//write_int(nsize);
	//write_str("\n");

	return nptr;
}*/

typedef struct {
	const char* text;
	int done;
} load_data;

/*const char* do_load_data(lua_State *L, void *ud, size_t *size) {
	load_data* d = ud;

	if (d->done)
		return NULL;

	d->done = 1;
	*size = strlen(d->text);
	return d->text;
}*/

void main() {
	write_str("Kernel starting...\n");

	heap_base = kernel_info_table->kernel_top;
	heap_top = kernel_info_table->memory_top;
	if (heap_top > 0x7FFFFFFF)
		heap_top = 0x7FFFFFFF;

	size_t heap_size = heap_top - heap_base;

	write_str("Heap base: ");
	write_hex(heap_base);
	write_str("\n");

	write_str("Heap top: ");
	write_hex(heap_top);
	write_str("\n");

	write_str("Heap size: ");
	write_hex(heap_size);
	write_str("\n");

	setup_heap(heap_base, heap_top);

	lua_State* state = luaL_newstate();
	luaL_openlibs(state);
	
	const char* test = "local x = 0 for i=1,1000 do x=x+i end print('reeeeeeeeeee') print(x)";

	write_str("Test script: ");
	write_str(test);
	write_str("\n");

	if (luaL_loadstring(state, test)) {
		write_str("Failed to load test script.");
		__halt();
	}

	lua_call(state, 0, 1);
	double res = lua_tonumber(state, -1);

	write_str("Result: ");
	write_int(res);
	write_str("\n");

	write_str("glua.team official operating system hurr\n");

	__halt();
}