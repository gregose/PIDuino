// PIDuino runtime'
// - TC4 interface
// - PID calcuation
// - opto22 line sensing

// lots of code from tc4-shield and aBourbon
#include <Wire.h>

// Third-party libraries
#include <Scheduler.h>
#include <OptoIn.h>
#include <PIDuinoTasks.h>

#include "PIDuino.h"

Scheduler __scheduler;

Blinker _blinker13(13, 500);
OptoStatusUpdater _opto_status_updater(50);

void setup()
{

  Serial.begin(BAUD);

  _blinker13.setup();
  _opto_status_updater.setup();

  __scheduler.setup();

  __scheduler.queue(&_blinker13);
  __scheduler.queue(&_opto_status_updater);
}

void loop()
{
  __scheduler.processMessages();
}
