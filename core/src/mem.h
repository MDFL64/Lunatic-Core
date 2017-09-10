#pragma once

typedef struct {
	void* base;
	void* cursor;
} mem_heap;

mem_heap mem_setup(size_t base, size_t top);

void* mem_alloc(mem_heap* heap, size_t size);

void mem_free(mem_heap* heap, void* ptr);

void mem_check(mem_heap* heap);