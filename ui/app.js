var express = require('express')
  , routes = require('./routes')
  , http = require('http')
  , path = require('path')
  , serialport = require("serialport")
  , SerialPort = serialport.SerialPort;
  
var app = express();

app.configure(function(){
  app.set('port', process.env.PORT || 3000);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'jade');
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
});

app.configure('development', function(){
  app.use(express.errorHandler());
});

app.get('/', routes.index);

var server = http.createServer(app).listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});

var io = require('socket.io').listen(server)

var sp = new SerialPort("/dev/ttyACM0", {
  baudrate: 57600,
  parser: serialport.parsers.readline("\r\n"),
});

// Inital swtich state
var switch_state = {
  power: false,
  brew: false,
  hotwater: false,
  steam: false        
};

io.sockets.on('connection', function (socket) {
  socket.emit('switch', switch_state);
});

sp.on("data", function(data) {
  var parts = data.split("|");
  console.log(parts);
  switch (parts[0]) {
    case("T"):
      io.sockets.emit('temp', {
        ambient: parseFloat(parts[1]),
        boiler: parseFloat(parts[2]),
        grouphead: parseFloat(parts[3])
      });
      break;
    case("K"):
      io.sockets.emit('knob', {
        value: parseFloat(parts[1])
      });
      break;      
    case("S"):
      switch_state = {
        power: parts[1] == "1",
        brew: parts[2] == "1",
        hotwater: parts[3] == "1",
        steam: parts[4] == "1"        
      };
      io.sockets.emit('switch', switch_state);
      break;
    case("P"):
      io.sockets.emit('pid', {
        dt: parseFloat(parts[1]),
        setpoint: parseFloat(parts[2]),
        intput: parseFloat(parts[3]),
        output: parseFloat(parts[4]),
        error: parseFloat(parts[5]),
        integral: parseFloat(parts[6]),
        derivative: parseFloat(parts[7]),
        kp: parseFloat(parts[8]),
        ki: parseFloat(parts[9]),
        kd: parseFloat(parts[10])              
      });
      break;      
  }
/*
  io.sockets.emit('serial', {
    duty: parts[0],
    time: parts[1],
    amb_temp: parts[2],
    t1_temp: parts[3],
    t1_ror: parts[4],
    t2_temp: parts[5],
    t2_ror: parts[6],
    sw_power: parseInt(parts[7]) == 0,
    sw_brew: parseInt(parts[8]) == 0,
    sw_hotwater: parseInt(parts[9]) == 0,
    sw_steam: parseInt(parts[10]) == 0
  });
*/
});