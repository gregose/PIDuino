#include "TempUpdater.h"

TempUpdater::TempUpdater(int period_in) {
  temp_monitor = TempMonitor();
  run_state = RUN_STATE_IDLE;
  period = period_in; // time in between temp reads
}

void TempUpdater::setup() {
  temp_monitor.setup();
}

void TempUpdater::next() {
  if(run_state < RUN_STATE_READ)
    run_state++;
  else
    run_state = RUN_STATE_IDLE;
}

void TempUpdater::run(Scheduler* scheduler) {
  int n_period = 0;

  //Serial.print("TempUpdater::state - ");
  //Serial.println(run_state);

  switch (run_state) {
    case RUN_STATE_IDLE:
      n_period = period;
      break;
    case RUN_STATE_CONVERT_BOILER:
      n_period = temp_monitor.convert(BOILER);
      break;
    case RUN_STATE_SAMPLE_BOILER:
      temp_monitor.sample(BOILER);
      break;
    case RUN_STATE_CONVERT_BREWGROUP:
      n_period = temp_monitor.convert(BREWGROUP);
      break;
    case RUN_STATE_SAMPLE_BREWGROUP:
      temp_monitor.sample(BREWGROUP);
      break;
    case RUN_STATE_READ:
      TempStatus * temp_status = temp_monitor.status();
      Serial.print(temp_status->ambient);
      Serial.print("|");
      Serial.print(temp_status->boiler);
      Serial.print("|");
      Serial.print(temp_status->brewgroup);
      Serial.println();
      break;
  }

  //Serial.print(" period: ");
  //Serial.println(n_period);

  next();
  if(n_period == 0) {
    scheduler->queue(this);
  } else {
    scheduler->schedule(this, n_period);
  }
}