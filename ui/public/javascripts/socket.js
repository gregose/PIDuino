var socket = io.connect('http://192.168.50.133:3000');
socket.on('serial', function (data) {
  $('#brew').text(data.t2_temp);
  $('#boiler').text(data.t1_temp);
  t2data.push(data.t2_temp);
  t1data.push(data.t1_temp);
  $('#power_sw').attr("class", data.sw_power ? "on" : "off");
  $('#brew_sw').attr("class", data.sw_brew ? "on" : "off");
  $('#hotwater_sw').attr("class", data.sw_hotwater ? "on" : "off");
  $('#steam_sw').attr("class", data.sw_steam ? "on" : "off");

});