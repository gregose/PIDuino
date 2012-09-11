// PIDuino runtime'
// - TC4 interface
// - PID calcuation
// - opto22 line sensing

// lots of code from tc4-shield and aBourbon
#include <Wire.h>

// Third-party libraries
#include <Scheduler.h>
#include <PIDuinoTasks.h>
#include <TempMonitor.h>

#include "PIDuino.h"

Scheduler scheduler;

Blinker blink_13(13, 500);
OptoStatusUpdater opto_status_updater(50);
TempUpdater temp_updater(2000);


void setup()
{
  delay(500);

  Wire.begin();
  Serial.begin(BAUD);


  blink_13.setup();
  opto_status_updater.setup();
  temp_updater.setup();

  scheduler.setup();

  scheduler.queue(&blink_13);
  scheduler.queue(&opto_status_updater);
  scheduler.queue(&temp_updater);
}

void loop()
{
  scheduler.processMessages();
}
