// unhealthy obsession with javascript intensifies

var fs = require("fs");
var child_process = require("child_process");

console.log("\n>> Building...");

var asm_output = child_process.execFileSync("fasm",["moonboot.asm"]).toString();
console.log(asm_output);

if (process.argv[2]=="noflash") return;

var bootloader_size = fs.statSync("moonboot.bin").size;
if (bootloader_size % 512 != 0)
	throw "Boot code is not an even number of sectors";
bootloader_size /= 512;

console.log(">> Locating destination drive...");
var drive = child_process.execFileSync("wmic",["volume","get","deviceid,driveletter,label"]).toString().match(/([^\s]+)\s+([A-Z]:)\s+LUNATIC/);
if (drive==null)
	throw "No drive.";

var drive_id_original = drive[1]
var drive_id = drive_id_original.replace("?",".").slice(0,-1);
var drive_letter = drive[2];

console.log("Using drive "+drive_letter+" -- "+drive_id+"\n");

console.log(">> Reading old boot sector from drive, performing sanity checks...");

var boot_sector = child_process.execFileSync("dd",["if="+drive_id,"count=2"],{stdio:["ignore", "pipe", "ignore"]});

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

var fd = fs.openSync("moonboot.bin","r+");
if (fs.readSync(fd, patch_sector, 0, 1024, 0) != 1024)
	throw "Failed to read moonboot.bin.";

boot_sector.copy(patch_sector,3,3,90);
boot_sector.copy(patch_sector,512,512,1024);

if (fs.writeSync(fd, patch_sector, 0, 1024, 0) != 1024)
	throw "Failed to write moonboot.bin.";

fs.closeSync(fd);

console.log("Done!\n");

console.log(">> Unmounting drive...");

child_process.execFileSync("mountvol",[drive_letter,"/D"]);

console.log("Hold on!\n");

setTimeout(function() {
	console.log(">> Writing to drive...");

	var out = child_process.execFileSync("dd",["if=moonboot.bin","of="+drive_id]).toString();
	console.log(out);

	console.log(">> Remounting drive...");

	child_process.execFileSync("mountvol",[drive_letter,drive_id_original]);

	console.log("Done!\n");
},1000);