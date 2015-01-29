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
#include <Settings.h>
#include <StatusLED.h>

#include "PIDuino.h"

PWM16 ssr;
Scheduler scheduler;
Settings settings;
StatusLED status_led(STATUS_LED_R, STATUS_LED_G, STATUS_LED_B);

Blinker blink_13(13, 1000);
TempUpdater temp_updater(1000, &settings);
PIDPot pid_pot(3, 1000);
PIDUpdater pid_updater(2000, &ssr, &temp_updater, &pid_pot, &settings);
OptoStatusUpdater opto_status_updater(50, &pid_updater);

void setup()
{
  Wire.begin();
  status_led.alpha(1);
  status_led.yellow();
  delayForYunBoot();
  status_led.on();
  status_led.blue();

  Serial1.begin(BAUD);

  // Output settings from eeprom
  settings.update();
  settings.log();

  /* Update settings w/ new amb offset - */
  //settings.data()->T_offset = -2.1;
  //settings.data()->brew_setpoint = 203.0;
  //settings.data()->steam_setpoint = 240.0;
  //
  //settings.save();

  //settings.update();
  //settings.log();
  /**/

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

void delayForYunBoot()
{
  int c = -1;

  // Delay until u-boot initiated
  delay(10000);
  status_led.red();

  Serial1.begin(115200); // Set the baud for u-boot.
  Serial.begin(115200); // output over CDC

  // wait for u-boot to finish startup.
  // consume all bytes until we are done.
  do {
    status_led.off();
    while (Serial1.available() > 0) {
      status_led.on();
      c = Serial1.read();
      if (c != -1) {
        Serial.write(c);
      }
    }
    delay(1000);
  } while (Serial1.available() > 0);
}
