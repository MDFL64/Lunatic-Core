#include "lauxlib.h"

#include "screen.h"

void start() {
	//write_str("Balls. ");
	//write_int(1337);
	//write_char(' ');
	//write_int(6969);
	
	lua_State* state = luaL_newstate();
	
	write_str("HELLO!\n");
	
	asm("hlt");
}