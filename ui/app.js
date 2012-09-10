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

io.sockets.on('connection', function (socket) {
  socket.emit('news', { hello: 'world' });
  socket.on('my other event', function (data) {
    console.log(data);
  });
});

var sp = new SerialPort("COM4", {
  baudrate: 57600,
  parser: serialport.parsers.readline("\r\n"),
});

sp.on("data", function(data) {
  var parts = data.split(",");
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
});