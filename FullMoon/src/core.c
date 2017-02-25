//#include <intrun.h>

__int8* rawmem = 0;

void main() {

	rawmem[0xB8000] = 'A';
	rawmem[0xB8002] = 'd';
	rawmem[0xB8004] = 'a';
	rawmem[0xB8006] = 'm';

	__halt();
	//printf("xyzzy");
}