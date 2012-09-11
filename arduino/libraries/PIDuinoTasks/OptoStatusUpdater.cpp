#include "OptoStatusUpdater.h"

OptoStatusUpdater::OptoStatusUpdater(int period)
{
  _period = period;
}

void OptoStatusUpdater::setup(){

}

void OptoStatusUpdater::run(Scheduler* scheduler)
{
  scheduler->schedule(this, _period);

  if(_opto_in.Update() == true) {
    OptoStatus * opto_status = _opto_in.Status();
    Serial.print(opto_status->ch0);
    Serial.print("|");
    Serial.print(opto_status->ch1);
    Serial.print("|");
    Serial.print(opto_status->ch2);
    Serial.print("|");
    Serial.print(opto_status->ch3);
    Serial.println();
  }
}