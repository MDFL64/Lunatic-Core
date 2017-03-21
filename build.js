var child_process = require("child_process");

var selected = process.argv[2];

var options = {};

var libs = [
	"LuaJIT/src/lib_base.c", // todo remove even more of this.
	"LuaJIT/src/lib_math.c",
	"LuaJIT/src/lib_bit.c",
	"LuaJIT/src/lib_string.c",
	"LuaJIT/src/lib_table.c",
	//"LuaJIT/src/lib_io.c",
	//"LuaJIT/src/lib_os.c",
	//"LuaJIT/src/lib_package.c",
	//"LuaJIT/src/lib_debug.c",
	//"LuaJIT/src/lib_jit.c",
	//"LuaJIT/src/lib_ffi.c"
];

var config = [
	"-D","LUAJIT_TARGET=LUAJIT_ARCH_X64",
	"-D","LUAJIT_OS=LUAJIT_OS_OTHER",

	"-D","LUAJIT_USE_SYSMALLOC",
	"-D","LUAJIT_NO_UNWIND",

	"-D","LUAJIT_DISABLE_JIT",
	"-D","LUAJIT_DISABLE_FFI",

	"-D","LUAJIT_ENABLE_GC64",

	"-D","LUA_USE_ASSERT",
	"-D","LUA_USE_APICHECK"
];

var common_kernel_args = [
	//"-std=gnu99",
		
	//"-Wall",
	//"-w",

	"-I","kernel/src",
	"-I","LuaJIT/src",

	//"-O3",

	"kernel/src/*.c",
	"LuaJIT/src/lj_*.c",
	"LuaJIT/src/lib_aux.c",
	"LuaJIT/src/lib_init.c",

	"-L","bin",
	"-llj_vm"
].concat(config).concat(libs);

options.kernel = function() {
	console.log(">> Building...");

	var compile_output = child_process.execFileSync("clang",[
		"-target","x86_64-pc-none-gnu",
		"-nostdlib",

		"-I","kernel/stdlib",
		"kernel/stdlib/*.c",

		"-e","start",
		"-Wl,--image-base,0x1000000,--subsystem,native,-Map=bin/FullMoon.map",

		//"-S",
		"-o","bin/FullMoon.krn"
	].concat(common_kernel_args)).toString();

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

/*options.userkernel = function() {
	var compile_output = child_process.execFileSync("clang",[
		"-target","x86_64-pc-windows-gnu",

		"-D","LUNATIC_USERMODE",

		"-Wl,-Map=bin/BadMoon.map",
		//"-S",
		"-o","bin/BadMoon.exe"
	].concat(common_kernel_args)).toString();

	console.log(">> Done!\n");
}*/

options.ljvm = function() {
	child_process.execFileSync("clang",[
		"-target","x86_64-pc-windows-gnu","LuaJIT/src/host/minilua.c","-o","bin/minilua.exe"
	]);
	child_process.execFileSync("bin/minilua",[ // "-D","JIT" "-D","FFI" "-D","WIN"
		"LuaJIT/dynasm/dynasm.lua","-LN","-D","NO_UNWIND","-o","LuaJIT/src/host/buildvm_arch.h","LuaJIT/src/vm_x64.dasc"
	]);

	child_process.execFileSync("clang",[
		"-target","x86_64-pc-windows-gnu","-I","LuaJIT/src","LuaJIT/src/host/buildvm*.c","-o","bin/buildvm.exe"
	].concat(config));
	child_process.execFileSync("bin/buildvm",[
		"-m","coffasm","-o","bin/lj_vm.asm"
	]);
	/*child_process.execFileSync("bin/buildvm",[
		"-m","peobj","-o","bin/liblj_vm.a"
	]);*/
	child_process.execFileSync("bin/buildvm",[
		"-m","libdef","-o","LuaJIT/src/lj_libdef.h"
	].concat(libs));
	child_process.execFileSync("bin/buildvm",[
		"-m","bcdef","-o","LuaJIT/src/lj_bcdef.h"
	].concat(libs));
	child_process.execFileSync("bin/buildvm",[
		"-m","ffdef","-o","LuaJIT/src/lj_ffdef.h"
	].concat(libs));
	child_process.execFileSync("bin/buildvm",[
		"-m","recdef","-o","LuaJIT/src/lj_recdef.h"
	].concat(libs));
	child_process.execFileSync("bin/buildvm",[
		"-m","folddef","-o","LuaJIT/src/lj_folddef.h","LuaJIT/src/lj_opt_fold.c"
	]);

	child_process.execFileSync("as",[
		"bin/lj_vm.asm","-o","bin/liblj_vm.a"
	]);
}

options.test = function() {
	child_process.execFileSync("C:\\Program Files\\Oracle\\VirtualBox\\VirtualBox.exe",[
		//"C:\\Program Files\\Oracle\\VirtualBox\\VirtualBox.exe",
		"--startvm","Lunatic","--debug"
	]);
}

selected = options[selected];

if (selected) {
	selected();
} else {
	console.log("Valid options: ",Object.keys(options));
}