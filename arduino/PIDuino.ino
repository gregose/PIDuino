// PIDuino runtime'
// - TC4 interface
// - PID calcuation
// - opto22 line sensing

// lots of code from tc4-shield and aBourbon
#include <Wire.h>

// Third-party libraries
#include <Scheduler.h>
#include <PWM16.h>
#include <PIDuinoTasks.h>
#include <TempMonitor.h>

#include "PIDuino.h"


PWM16 ssr;

Scheduler scheduler;

Blinker blink_13(13, 500);
TempUpdater temp_updater(1000);
PIDPot pid_pot(3, 1000);
PIDUpdater pid_updater(2000, &ssr, &temp_updater, &pid_pot);
OptoStatusUpdater opto_status_updater(50, &pid_updater);

void setup()
{
  Wire.begin();
  Serial.begin(BAUD);

  // SSR, uses timer1 to schedule duty cycle
  ssr.Setup(SSR_FREQ);
  ssr.Out(0, 0); // init to 0% duty, OT2 always 0% for now

  blink_13.setup();
  pid_pot.setup();
  temp_updater.setup();
  pid_updater.setup();
  opto_status_updater.setup();

  scheduler.setup();

  scheduler.queue(&blink_13);
  scheduler.queue(&pid_pot);
  scheduler.queue(&opto_status_updater);
  scheduler.queue(&temp_updater);
  scheduler.queue(&pid_updater);
}

void loop()
{
  scheduler.processMessages();
}
