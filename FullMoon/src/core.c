__int8* video_memory = 0xB8000;

const char* hello = "Kernel here. Hi! This took way too much effort.";

void print(const char* str) {
	int i = 0;
	while (str[i]) {
		video_memory[i * 2] = str[i];
		i++;
	}
}

void main() {

	print(hello);

	__halt();
}