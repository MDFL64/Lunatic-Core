var child_process = require("child_process");

console.log(">> Building...");

var compile_output = child_process.execFileSync("clang",[
	"-std=gnu99","-Wall",
	//"-masm=intel","-S",

	"-I","kernel/stdlib",
	"-include","default.h",
	
	"-I","LuaJIT/src",

	"-target","x86_64-pc-none-gnu",
	"-nostdlib",

	"-O3",
	//"-save-temps=obj",

	"kernel/src/*.c","kernel/stdlib/*.c",

	"-e","start",
	"-Wl,--image-base,0x1000000,--subsystem,native",

	"-o","kernel/FullMoon.krn"
]).toString();

console.log(compile_output);

child_process.execFileSync("strip",["kernel/FullMoon.krn","-R",".comment"]);

console.log(">> Locating destination drive...\n");
var drive = child_process.execFileSync("wmic",["volume","get","deviceid,driveletter,label"]).toString().match(/([^\s]+)\s+([A-Z]:)\s+LUNATIC/);
if (drive==null)
	throw "No drive.";

var drive_letter = drive[2];

console.log(">> Copying...\n");
child_process.execFileSync("xcopy",["kernel\\FullMoon.krn",drive_letter+"\\Lunatic\\init\\","/Y"]);

console.log(">> Done!\n");