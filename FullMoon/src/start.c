#include <screen.h>
#include <string.h>

#include "../../Lua/lua.h"

struct {
	unsigned __int64 memory_top;
	unsigned __int64 kernel_top;
	unsigned __int32 init_file_count;
}* kernel_info_table = 0x60000;

size_t heap_base;
size_t heap_top;

void* moon_alloc(void* ud, void* optr, size_t osize, size_t nsize) {
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
		memcpy(nptr, optr, osize);

	write_str("ALLOCATE ");
	write_int(nsize);
	write_str("\n");

	return nptr;
}

typedef struct {
	const char* text;
	int done;
} load_data;

const char* do_load_data(lua_State *L, void *ud, size_t *size) {
	write_str("LOAD!\n");
	load_data* d = ud;

	if (d->done)
		return NULL;

	d->done = 1;
	*size = strlen(d->text);
	return d->text;
}

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

	/*for (;;) {
		__int8* x = moon_alloc(1024);

		write_str("> ");
		write_hex(x);
		write_str("\n");

		//for (size_t y = 0; y < 10000000; y++);
	}*/
	lua_State* state = lua_newstate(moon_alloc, 0);

	write_str("...\n");

	load_data d;
	d.text = "return 2+2";
	d.done = 0;

	int r = 12;
	r = lua_load(state, do_load_data, &d, "test", NULL);

	write_str("result = ");
	write_int(r);
	write_str("\n...\n");

	__halt();
}