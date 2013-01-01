var socket = io.connect('http://' + document.domain + ':3000');

socket.on('temp', function (data) {
  $('#brew').text(data.grouphead);
  $('#boiler').text(data.boiler);
  t2data.push(data.grouphead);
  t1data.push(data.boiler);
});

socket.on('pid', function (data) {
  $('#brew_detail').show();
  $('#temp-setpoint').text(data.setpoint);
  $('#temp-error').text('(' + data.error + ')');
  $('#output').text(data.output + '%');

  if(data.error > 2.0) { 
    $('#temp-status').text('(heating)');
  } else if(data.error < -2.0) {
    $('#temp-status').text('(cooling)');
  } else {
    $('#temp-status').text('(ready)');
  }
});

socket.on('switch', function (data) {
  console.log(data.power == false);
  if(data.power == false) {
    $('#temp-mode').text('Idle');
      $('#temp-status').text('');
    $('#brew_detail').hide();
  }
   else if(data.steam == true) {
    $('#temp-mode').text('Steam');    
  } else {
    $('#temp-mode').text('Brew');       
  }

  if(data.brew == true) {
    startTimer();
  } else if(data.brew == false) {
    stopTimer();
  } 
});

/*
function update() { 
  $("#board").append(".");
  t2data.push(100);
  t1data.push(Math.random() * 100 + 143);
  setTimeout('update()',500);
}

update();
*/