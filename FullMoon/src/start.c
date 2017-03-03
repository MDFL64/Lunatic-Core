#include "screen.h"

struct {
	__int64 memory_top;
	__int32 init_file_count;
}* kernel_info_table = 0x60000;

void main() {
	write_str("Kernel starting...\n");

	write_str("Approx. Memory: ");
	write_int(kernel_info_table->memory_top); //kernel_info_table->memory
	write_str(" bytes\n");

	__halt();
}