//#include <intrun.h>

void main() {
	int x = 0xBEEF;
	int y = 0x1234;
	int z = x + y;

	//__int32* a = 0;
	//__int32* b = 4;
	int a = *(int*)0;
	int b = *(int*)4;
	int c = *(int*)7;

	__halt();
	//printf("xyzzy");
}