
// Hopefully this is the actual terminal size.
const int TERM_W = 80;
const int TERM_H = 25;

int term_x = 0;
int term_y = 0;

__int8* raw_memory = 0;

__int8* video_memory = 0xB8000;

const char* hello = "Kernel started.";

void print(const char* str) {
	int i = 0;
	while (str[i]) {
		video_memory[i * 2] = str[i];
		i++;
	}

	//__outbyte(0x3D4, 0);
}

void printx() {
	for (int i = 0; i < TERM_W * TERM_H - 2; i++) {
		video_memory[i * 2] = '@';
	}
}

void main() {

	printx();
	print(hello);

	__halt();
}