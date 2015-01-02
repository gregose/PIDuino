#include "OptoStatusUpdater.h"

OptoStatusUpdater::OptoStatusUpdater(int period_in, PIDUpdater* pid_in = NULL)
{
  period = period_in;
  opto_in = OptoIn();
  pid = pid_in;
}

void OptoStatusUpdater::setup(){
  opto_in.setup();
}

void OptoStatusUpdater::run(Scheduler* scheduler)
{
  scheduler->schedule(this, period);

  if(opto_in.update() == true) {
    OptoStatus * opto_status = opto_in.status();

    // Set appropiate SSR state
    if(opto_status->ch0) {
      if(opto_status->ch1 || opto_status->ch2) // we are either brewing or steaming
        pid->disable(100); // disable PID, max duty for pump
      else
        pid->enable();
    } else {
      pid->disable(0); // set output off
    }

    // Set correct temp based on steam switch or not
    if(opto_status->ch3)
      pid->steamSetPoint();
    else
      pid->brewSetPoint();

    // Output status
    Serial1.print("S|");
    Serial1.print(opto_status->ch0);
    Serial1.print("|");
    Serial1.print(opto_status->ch1);
    Serial1.print("|");
    Serial1.print(opto_status->ch2);
    Serial1.print("|");
    Serial1.print(opto_status->ch3);
    Serial1.println();
  }
}
