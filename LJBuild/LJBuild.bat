gcc ../LuaJIT/src/host/minilua.c -o minilua

minilua ../LuaJIT/dynasm/dynasm.lua -LN -D P64 -o ../LuaJIT/src/host/buildvm_arch.h ../LuaJIT/src/vm_x86.dasc

gcc ../LuaJIT/src/host/buildvm*.c -o buildvm -I ../LuaJIT/src

buildvm -m peobj -o lj_vm.obj

buildvm -m libdef -o ../LuaJIT/src/lj_libdef.h ../LuaJIT/src/lib_base.c ../LuaJIT/src/lib_math.c ../LuaJIT/src/lib_bit.c ../LuaJIT/src/lib_string.c ../LuaJIT/src/lib_table.c ../LuaJIT/src/lib_ffi.c

buildvm -m bcdef -o ../LuaJIT/src/lj_bcdef.h ../LuaJIT/src/lib_base.c ../LuaJIT/src/lib_math.c ../LuaJIT/src/lib_bit.c ../LuaJIT/src/lib_string.c ../LuaJIT/src/lib_table.c ../LuaJIT/src/lib_ffi.c

buildvm -m ffdef -o ../LuaJIT/src/lj_ffdef.h ../LuaJIT/src/lib_base.c ../LuaJIT/src/lib_math.c ../LuaJIT/src/lib_bit.c ../LuaJIT/src/lib_string.c ../LuaJIT/src/lib_table.c ../LuaJIT/src/lib_ffi.c