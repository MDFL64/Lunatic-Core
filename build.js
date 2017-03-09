var child_process = require("child_process");

var selected = process.argv[2];

var options = {};

var libs = [
	"LuaJIT/src/lib_base.c",
	"LuaJIT/src/lib_math.c",
	"LuaJIT/src/lib_string.c",
	"LuaJIT/src/lib_table.c",
	"LuaJIT/src/lib_bit.c",
	"LuaJIT/src/lib_ffi.c"
];

var config = [
	"-D","LUAJIT_USE_SYSMALLOC",
	"-D","LUAJIT_DISABLE_JIT",
	"-D","LUAJIT_DISABLE_FFI"
];

options.kernel = function() {
	console.log(">> Building...");

	var compile_output = child_process.execFileSync("clang",[
		"-std=gnu99",
		
		//"-Wall",
		"-w",

		"-I","kernel/src",
		"-I","kernel/stdlib",
		"-I","LuaJIT/src",

		"-target","x86_64-pc-none-gnu",
		"-nostdlib",

		//"-O3",

		"kernel/src/*.c",
		"kernel/stdlib/*.c",
		"LuaJIT/src/lj_*.c",

		"-L","bin",
		"-llj_vm",

		"-e","start",
		"-Wl,--image-base,0x1000000,--subsystem,native",

		"-o","bin/FullMoon.krn"
	].concat(config)).toString();

	console.log(compile_output);

	child_process.execFileSync("strip",["bin/FullMoon.krn","-R",".comment"]);

	console.log(">> Locating destination drive...\n");
	var drive = child_process.execFileSync("wmic",["volume","get","deviceid,driveletter,label"]).toString().match(/([^\s]+)\s+([A-Z]:)\s+LUNATIC/);
	if (drive==null)
		throw "No drive.";

	var drive_letter = drive[2];

	console.log(">> Copying...\n");
	child_process.execFileSync("xcopy",["bin\\FullMoon.krn",drive_letter+"\\Lunatic\\init\\","/Y"]);

	console.log(">> Done!\n");
}

options.ljvm = function() {
	child_process.execFileSync("clang",[
		"-target","x86_64-pc-windows-gnu","LuaJIT/src/host/minilua.c","-o","bin/minilua.exe"
	]);
	child_process.execFileSync("bin/minilua",[
		"LuaJIT/dynasm/dynasm.lua","-LN","-D","P64","-o","LuaJIT/src/host/buildvm_arch.h","LuaJIT/src/vm_x86.dasc"
	]);

	child_process.execFileSync("clang",[
		"-target","x86_64-pc-windows-gnu","-I","LuaJIT/src","LuaJIT/src/host/buildvm*.c","-o","bin/buildvm.exe"
	].concat(config));
	child_process.execFileSync("bin/buildvm",[
		"-m","coffasm","-o","bin/lj_vm.asm"
	]);
	child_process.execFileSync("bin/buildvm",[
		"-m","libdef","-o","LuaJIT/src/lj_libdef.h"
	].concat(libs));
	child_process.execFileSync("bin/buildvm",[
		"-m","bcdef","-o","LuaJIT/src/lj_bcdef.h"
	].concat(libs));
	child_process.execFileSync("bin/buildvm",[
		"-m","ffdef","-o","LuaJIT/src/lj_ffdef.h"
	].concat(libs));

	child_process.execFileSync("as",[
		"bin/lj_vm.asm","-o","bin/liblj_vm.a"
	]);
}

selected = options[selected];

if (selected) {
	selected();
} else {
	console.log("Valid options: ",options);
}