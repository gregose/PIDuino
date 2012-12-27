var serialport = require("serialport")

serialport.list(function (err, ports) {
	ports.forEach(function(port) {
	  console.log(port.comName);
	  console.log(port.pnpId);
	  console.log(port.manufacturer);
	});
});