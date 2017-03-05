#include "screen.h"

#include "lualib.h"

LUALIB_API int luaopen_io(lua_State *L) {
	write_str("io\n");
	__halt();
	return 0;
}

LUALIB_API int luaopen_os(lua_State *L) {
	write_str("os\n");
	__halt();
	return 0;
}

LUALIB_API int luaopen_package(lua_State *L) {
	write_str("package\n");
	__halt();
	return 0;
}

LUALIB_API int luaopen_debug(lua_State *L) {
	write_str("debug\n");
	__halt();
	return 0;
}

LUALIB_API int luaopen_jit(lua_State *L) {
	write_str("jit\n");
	__halt();
	return 0;
}

void lj_err_unwind_win64(void *rec, void *cf, void *ctx, void *dispatch) {
	write_str("TIME TO UNWIND\n");
	__halt();
}