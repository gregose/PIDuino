#include <Scheduler.h>

#include "PIDPot.h"

PIDPot::PIDPot(int pin, int period)
{
  _pin = pin;
  _period = period;
  _val = 2;
}

void PIDPot::setup()
{

}

void PIDPot::run(Scheduler* scheduler)
{
  scheduler->schedule(this, _period);
  
  _val = map(analogRead(_pin), 0, 1023, 0, 100);
  Serial.print("K|");
  Serial.println(_val);
}

double PIDPot::val()
{
  return (double)_val;
}