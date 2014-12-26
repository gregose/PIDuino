var timer;
var start = 0;

function updateTimer() {
  $(".time").text(((new Date().getTime() - start) / 1000).toFixed(1));
  timer = setTimeout("updateTimer()",100);
}

function startTimer() {
	start = new Date().getTime();

  if (timer > 0) {
    return;
  }

  updateTimer();	
}

function stopTimer() {
  clearTimeout(timer);
  timer = 0;
}

//startTimer();