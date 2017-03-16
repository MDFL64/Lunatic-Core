#include <lualib.h>

#include "screen.h"

LUALIB_API int luaopen_io(lua_State *L) {
	return 0;
}

LUALIB_API int luaopen_os(lua_State *L) {
	return 0;
}

LUALIB_API int luaopen_package(lua_State *L) {
	return 0;
}

LUALIB_API int luaopen_debug(lua_State *L) {
	return 0;
}

LUALIB_API int luaopen_jit(lua_State *L) {
	return 0;
}

// this is just an exception handler, it should never actually get called.
void lj_err_unwind_win(void* rec,void* cf, void* ctx, void* dispatch) {
	write_str_halt("Windows exception handler called. WTF?\n");
}