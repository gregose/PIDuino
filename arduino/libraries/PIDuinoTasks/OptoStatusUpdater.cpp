#include "OptoStatusUpdater.h"

OptoStatusUpdater::OptoStatusUpdater(int period_in, PWM16* ssr_in = NULL)
{
  period = period_in;
  ssr = ssr_in;
  opto_in = OptoIn();
}

void OptoStatusUpdater::setup(){
  opto_in.Setup();
}

void OptoStatusUpdater::run(Scheduler* scheduler)
{
  scheduler->schedule(this, period);

  if(opto_in.Update() == true) {
    OptoStatus * opto_status = opto_in.Status();

    // Set appropiate SSR state
    if(ssr && opto_status->ch0)
      ssr->Out(100,0);
    if(ssr && !(opto_status->ch0))
      ssr->Out(0,0);

    // Output status
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