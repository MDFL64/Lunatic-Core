#include <stddef.h>
#include "screen.h"

#include "mem.h"

typedef struct {
	unsigned int size;
	char used;
	char reserved[3];
} mem_info;

typedef struct {
	mem_info prev;
	mem_info next;
} mem_node;

mem_heap mem_setup(size_t base, size_t top) {
	if (sizeof(mem_node) != 16)
		write_str_halt("Memory node is badly sized!");

	while (base % 16 != 0)
		base++;
	
	while (top % 16 != 0)
		top--;
	
	top -= 16;

	if (base>=top)
		write_str_halt("Not enough room for heap!");
	
	size_t size = (top-base)/16 - 1;
	write_str(">> ");
	write_int(size);
	write_str("\n");

	mem_node* base_node = (mem_node*)base;
	mem_node* top_node = (mem_node*)top;

	base_node->next.size = size;
	base_node->next.used = 0;
	base_node->prev.size = 0;

	top_node->prev.size = size;
	top_node->prev.used = 0;
	top_node->next.size = 0;

	mem_heap result = {
		base_node,
		base_node
	};

	return result;
}

void* mem_alloc(mem_heap* heap, size_t size) {
	//asm("hlt");
	mem_node* cursor = heap->cursor; // rbp - 18

	char reset = 0;

	size_t req_size = size / 16;
	if (size%16 != 0)
		req_size++;

	for (;;) {
		if (cursor->next.size == 0) {
			if (reset) {
				write_str_halt("Out of memory!");
			} else {
				cursor = heap->base;
				reset = 1;
			}
		}

		if (!cursor->next.used && cursor->next.size >= req_size) {

			// block will work!
			size_t available_size = cursor->next.size;

			if (available_size > req_size+1) {
				// big fit, bisect this chunk
				size_t remaining_size = available_size - req_size - 1;
				if (remaining_size == 0)
					write_str_halt("Invalid remaining size!");

				mem_node* back = cursor + (1+available_size);
				mem_node* middle = cursor + (1+req_size);
				
				cursor->next.size = req_size;
				middle->prev.size = cursor->next.size;

				middle->next.size = remaining_size;
				back->prev.size = middle->next.size;

				middle->next.used = 0;
				middle->prev.used = 0;
			}

			void* ptr = 1+cursor;
			
			cursor->next.used = 1;
			cursor += (1+cursor->next.size);
			cursor->prev.used = 1;

			heap->cursor = cursor;
			return ptr;
		}

		cursor += (1+cursor->next.size);
	}
}