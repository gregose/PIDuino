#include "TempUpdater.h"

TempUpdater::TempUpdater(int idle_time_in, Settings* settings_in) {
  temp_monitor = new TempMonitor(settings_in);
  last_temp_status = new TempStatus();
  run_state = RUN_STATE_IDLE;
  idle_time = idle_time_in; // time in between temp reads
}

void TempUpdater::setup() {
  temp_monitor->setup();
}

void TempUpdater::next() {
  if(run_state < RUN_STATE_READ)
    run_state++;
  else
    run_state = RUN_STATE_IDLE;
}

TempStatus* TempUpdater::lastTemp() {
  return last_temp_status;
}

void TempUpdater::run(Scheduler* scheduler) {
  int n_period = 0;

  //Serial.print("TempUpdater::state - ");
  //Serial.println(run_state);

  switch (run_state) {
    case RUN_STATE_IDLE:
      n_period = idle_time;
      break;
    case RUN_STATE_CONVERT_BOILER:
      n_period = temp_monitor->convert(BOILER);
      break;
    case RUN_STATE_SAMPLE_BOILER:
      temp_monitor->sample(BOILER);
      break;
    case RUN_STATE_CONVERT_BREWGROUP:
      n_period = temp_monitor->convert(BREWGROUP);
      break;
    case RUN_STATE_SAMPLE_BREWGROUP:
      temp_monitor->sample(BREWGROUP);
      break;
    case RUN_STATE_READ:
      TempStatus * temp_status = temp_monitor->status();

      Serial1.print("T|");
      Serial1.print(temp_status->ambient);
      Serial1.print("|");
      Serial1.print(temp_status->boiler);
      Serial1.print("|");
      Serial1.print(temp_status->brewgroup);
      Serial1.println();

      // For PID updates
      last_temp_status->ambient = temp_status->ambient;
      last_temp_status->boiler = temp_status->boiler;
      last_temp_status->brewgroup = temp_status->brewgroup;

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
