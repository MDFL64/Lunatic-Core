void* memcpy(void* dest, const void* src, size_t count) {
	char* dest_byte = dest;
	const char* src_byte = src;

	while (count > 0) {
		*dest_byte = *src_byte;
		dest_byte++;
		src_byte++;
		count--;
	}

	return dest;
}