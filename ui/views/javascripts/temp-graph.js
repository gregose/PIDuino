$(function() {
  // Init switch display
  $('#temp-mode').text('Idle');
  $('#temp-status').text('');
  $('#brew_detail').show();

  // Init temp graph
  var now = Math.floor((new Date).getTime()/1000);
  var data = [
    { label: 'Boiler', values: [ { time: now, y: 0 } ] },
    { label: 'Grouphead', values: [ { time: now, y: 0 } ] }
  ];

  var tempGraph = $('#temp-graph').epoch({
    type: 'time.line',
    data: data,
    ticks: { time: 30, right: 10, left: 10 },
    axes: ['left', 'right', 'bottom'],
    windowSize: 180,
    margins: { top: 10, right: 30, bottom: 30, left: 30 }
  });

  // Handle incoming messages
  var websocket = new WebSocket("ws://"+document.location.host+"/events");
  websocket.onmessage = function(msg){
    var data = JSON.parse(msg.data);
    var timestamp = Math.floor(data.Header.Timestamp / 1000);
    console.log(data);
    switch(data.Header.Type) {
    case "Temp":
      $('#brew').text(data.GroupHead);
      $('#boiler').text(data.Boiler);
      tempGraph.push([
        { time: timestamp, y: data.Boiler },
        { time: timestamp, y: data.GroupHead }
      ]);
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
});
