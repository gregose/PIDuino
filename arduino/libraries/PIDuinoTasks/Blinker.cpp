#include <Scheduler.h>

#include "Blinker.h"

Blinker::Blinker(int pin, int period)
{
  _pin = pin;
  _state = 0;
  _period = period;
}

void Blinker::setup()
{
  pinMode( _pin, OUTPUT);  
}

void Blinker::run(Scheduler* scheduler)
{
  scheduler->schedule(this, _period);
  
  _state = (_state>0)?0:1;  
  digitalWrite(_pin, _state);
}