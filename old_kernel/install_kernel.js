var child_process = require("child_process");

console.log(">> Locating destination drive...");
var drive = child_process.execFileSync("wmic",["volume","get","deviceid,driveletter,label"]).toString().match(/([^\s]+)\s+([A-Z]:)\s+LUNATIC/);
if (drive==null)
	throw "No drive.";

var drive_letter = drive[2];

console.log(">> Copying...");
child_process.execFileSync("xcopy",["bin\\FullMoon.krn",drive_letter+"\\Lunatic\\init\\","/Y"]);

console.log(">> Done!");