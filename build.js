// unhealthy obsession with javascript intensifies

var child_process = require("child_process");

var selected = process.argv[2];

var options = {};

options.loader = function() {

	var fs = require("fs");
	var child_process = require("child_process");

	console.log("\n>> Building...");

	var asm_output = child_process.execFileSync("loader/fasm",["loader/NewMoon.asm"]).toString();
	console.log(asm_output);

	if (process.argv[2]=="noflash") return;

	var bootloader_size = fs.statSync("loader/NewMoon.bin").size;
	if (bootloader_size % 512 != 0)
		throw "Boot code is not a whole number of sectors";
	bootloader_size /= 512;

	console.log(">> Locating destination drive...");
	var drive = child_process.execFileSync("wmic",["volume","get","deviceid,driveletter,label"]).toString().match(/([^\s]+)\s+([A-Z]:)\s+LUNATIC/);
	if (drive==null)
		throw "No drive.";

	var drive_id_original = drive[1];
	var drive_id = drive_id_original.replace("?",".").slice(0,-1);
	var drive_letter = drive[2];

	console.log("Using drive "+drive_letter+" -- "+drive_id+"\n");

	console.log(">> Reading old boot sector from drive, performing sanity checks...");

	var boot_sector = child_process.execFileSync("loader/dd",["if="+drive_id,"count=2"],{stdio:["ignore", "pipe", "ignore"]});

	if (boot_sector.length != 1024)
		throw "Failed to read whole boot sector.";

	// SANITY CHECKS!
	if (boot_sector.toString("ascii",0x52,0x5A) != "FAT32   ")
		throw "Sanity check failed, not FAT32.";

	if (boot_sector.readInt16LE(0x0B) != 512)
		throw "Sanity check failed, sector size is wrong.";

	if (boot_sector.readInt16LE(0x30) != 1)
		throw "Sanity check failed, FSI is in the wrong sector.";

	if (boot_sector.readInt32LE(512) != 0x41615252)
		throw "Sanity check failed, FSI signature #1 is wrong.";

	if (boot_sector.readInt32LE(512+484) != 0x61417272)
		throw "Sanity check failed, FSI signature #2 is wrong.";

	var reserved_sectors = boot_sector.readInt16LE(0x0E);
	console.log("We will use "+bootloader_size+" / "+reserved_sectors+" reserved sectors.\n");
	if (bootloader_size > reserved_sectors)
		throw "Bootloader is too large to fit in reserved region.";

	console.log(">> Patching boot code...");

	var patch_sector = new Buffer(1024);

	var fd = fs.openSync("loader/NewMoon.bin","r+");
	if (fs.readSync(fd, patch_sector, 0, 1024, 0) != 1024)
		throw "Failed to read NewMoon.bin.";

	boot_sector.copy(patch_sector,3,3,90);
	boot_sector.copy(patch_sector,512,512,1024);

	if (fs.writeSync(fd, patch_sector, 0, 1024, 0) != 1024)
		throw "Failed to write NewMoon.bin.";

	fs.closeSync(fd);

	console.log("Done!\n");

	console.log(">> Unmounting drive...");

	child_process.execFileSync("mountvol",[drive_letter,"/D"]);

	console.log("Hold on!\n");

	setTimeout(function() {
		console.log(">> Writing to drive...");

		var out = child_process.execFileSync("loader/dd",["if=loader/NewMoon.bin","of="+drive_id]).toString();
		console.log(out);

		console.log(">> Remounting drive...");

		child_process.execFileSync("mountvol",[drive_letter,drive_id_original]);

		console.log("Done!\n");
	},1000);
}

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
	"LuaJIT/src/lib_ffi.c"
];

var config = [
	"-D","LUAJIT_TARGET=LUAJIT_ARCH_X64",
	"-D","LUAJIT_OS=LUAJIT_OS_OTHER",

	"-D","LUAJIT_USE_SYSMALLOC",
	"-D","LUAJIT_NO_UNWIND",

	"-D","LUAJIT_DISABLE_JIT",
	//"-D","LUAJIT_DISABLE_FFI",

	"-D","LUAJIT_ENABLE_GC64",

	"-D","LUA_USE_ASSERT",
	"-D","LUA_USE_APICHECK"
];

var common_core_args = [
	//"-std=gnu99",
		
	//"-Wall",
	//"-w",

	"-I","core/src",
	"-I","LuaJIT/src",

	//"-O3",

	"core/src/*.c",
	"LuaJIT/src/lj_*.c",
	"LuaJIT/src/lib_aux.c",
	"LuaJIT/src/lib_init.c",

	"-L","bin",
	"-llj_vm"
].concat(config).concat(libs);

options.core = function() {
	console.log(">> Building...");

	var compile_output = child_process.execFileSync("clang",[
		"-target","x86_64-pc-none-gnu",
		"-nostdlib",

		"-I","core/stdlib",
		"core/stdlib/*.c",

		"-e","start",
		"-Wl,--image-base,0x1000000,--subsystem,native,-Map=bin/FullMoon.map",

		//"-S",
		"-o","bin/FullMoon.exe"
	].concat(common_core_args)).toString();

	console.log(compile_output);

	child_process.execFileSync("strip",["bin/FullMoon.exe","-R",".comment"]);

	console.log(">> Locating destination drive...\n");
	var drive = child_process.execFileSync("wmic",["volume","get","deviceid,driveletter,label"]).toString().match(/([^\s]+)\s+([A-Z]:)\s+LUNATIC/);
	if (drive==null)
		throw "No drive.";

	var drive_letter = drive[2];

	console.log(">> Copying...\n");
	child_process.execFileSync("xcopy",["bin\\FullMoon.exe",drive_letter+"\\Lunatic\\Init\\","/Y"]);

	console.log(">> Done!\n");
}

/*options.userkernel = function() {
	var compile_output = child_process.execFileSync("clang",[
		"-target","x86_64-pc-windows-gnu",

		"-D","LUNATIC_USERMODE",

		"-Wl,-Map=bin/BadMoon.map",
		//"-S",
		"-o","bin/BadMoon.exe"
	].concat(common_core_args)).toString();

	console.log(">> Done!\n");
}*/

options.ljvm = function() {
	child_process.execFileSync("clang",[
		"-target","x86_64-pc-windows-gnu","LuaJIT/src/host/minilua.c","-o","bin/minilua.exe"
	]);
	child_process.execFileSync("bin/minilua",[ // "-D","JIT" "-D","FFI" "-D","WIN"
		"LuaJIT/dynasm/dynasm.lua","-LN","-D","FFI","-D","NO_UNWIND","-o","LuaJIT/src/host/buildvm_arch.h","LuaJIT/src/vm_x64.dasc"
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