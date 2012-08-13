var socket = io.connect('http://192.168.50.134:3000');
socket.on('serial', function (data) {
  $('#brew').text(data.t1_temp);
  $('#boiler').text(data.t2_temp);
});