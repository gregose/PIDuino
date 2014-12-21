var websocket = new WebSocket("ws://"+document.location.host+"/ws");

// Init switch display
$('#temp-mode').text('Idle');
$('#temp-status').text('');
$('#brew_detail').hide();

websocket.onmessage = function(msg){
  data = JSON.parse(msg.data);
  console.log(data);
  switch(data.Type) {
  case "Temp":
    $('#brew').text(data.GroupHead);
    $('#boiler').text(data.Boiler);
    t2data.push(data.GroupHead);
    t1data.push(data.Boiler);
    break
  case "PID":
    $('#brew_detail').show();
    $('#temp-setpoint').text(data.SetPoint);
    $('#temp-error').text('(' + data.Error + ')');
    $('#output').text(data.Output + '%');

    if(data.Error > 2.0) { 
      $('#temp-status').text('(heating)');
    } else if(data.Error < -2.0) {
      $('#temp-status').text('(cooling)');
    } else {
      $('#temp-status').text('(ready)');
    }
    break
  case "SwitchState":
    if(data.Power == false) {
      $('#temp-mode').text('Idle');
      $('#temp-status').text('');
      $('#brew_detail').hide();
    }
    else if(data.Steam == true) {
      $('#temp-mode').text('Steam');    
    } else {
      $('#temp-mode').text('Brew');       
    }

    if(data.Brew == true) {
      startTimer();
    } else if(data.Brew == false) {
      stopTimer();
    } 
  }
};
