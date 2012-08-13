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
    time: parts[0],
    amb_temp: parts[1],
    t1_temp: parts[2],
    t1_ror: parts[3],
    t2_temp: parts[4],
    t2_ror: parts[5]
  });
});