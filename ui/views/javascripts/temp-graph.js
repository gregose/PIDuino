$(function() {
  var debugMode = window.location.hash == "#debug";
  // side menu
  var menu = $(".menu-link").bigSlide({
    menuWidth: "800px",
    side: "right",
  });

  $(".push").on("click", function(e){
    if(e.target.className != "menu-link" && menu._state === "open") {
      menu.close();
    }
  });

  // Init switch display
  $("#temp-mode").text("Idle");
  $("#temp-status").text("");
  $("#brew_detail").hide();

  // Get temp history
  $.getJSON("/buffer.json", function(buffer) {

    // Init graph w/ temp history buffer
    var secondsPerTick = Math.floor(buffer.MaxLogSize / 6);
    var tempGraph = $("#temp-graph").epoch({
      type: "time.line",
      data: buffer.Data,
      ticks: { time: secondsPerTick, right: 10, left: 10 },
      axes: ["left", "right", "bottom"],
      windowSize: buffer.MaxLogSize,
      historySize: buffer.MaxLogSize,
      margins: { top: 0, right: 35, bottom: 30, left: 35 }
    });

    // Handle incoming messages
    var websocket = new WebSocket("ws://"+document.location.host+"/events");
    websocket.onmessage = function(msg){
      var data = JSON.parse(msg.data);
      var timestamp = Math.floor(data.Header.Timestamp / 1000);
      if(debugMode) {
        console.log(data);
      }
      switch(data.Header.Type) {
      case "Temp":
        $("#brew").text(data.Grouphead.toFixed(1));
        $("#boiler").text(data.Boiler.toFixed(1));
        tempGraph.push([
          { time: timestamp, y: data.Boiler },
          { time: timestamp, y: data.Grouphead }
        ]);
        break
      case "PID":
        $("#brew_detail").show();
        $("#temp-setpoint").text(data.SetPoint);
        $("#temp-error").text("(" + data.Error + ")");
        $("#output").text(data.Output + "%");

        if(data.Error > 2.0) {
          $("#temp-status").text("(heating)");
        } else if(data.Error < -2.0) {
          $("#temp-status").text("(cooling)");
        } else {
          $("#temp-status").text("(ready)");
        }
        break
      case "SwitchState":
        if(data.Power == false) {
          $("#temp-mode").text("Idle");
          $("#temp-status").text("");
          $("#brew_detail").hide();
        }
        else if(data.Steam == true) {
          $("#temp-mode").text("Steam");
        } else {
          $("#temp-mode").text("Brew");
        }

        if(data.Brew == true) {
          startTimer();
        } else if(data.Brew == false) {
          stopTimer();
        }
      }
    };
  });
});
