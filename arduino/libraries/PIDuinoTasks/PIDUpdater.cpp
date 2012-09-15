#include "PIDUpdater.h"

PIDUpdater::PIDUpdater(int period_in, PWM16* ssr_in, TempUpdater* temp_in) {
	temp = temp_in;
  ssr = ssr_in;
  period = period_in;

	input = 0;
	output = 0;
	setpoint = 100.00;
	enabled = false;
																				//k: P  I  D
	pid = PID(&input, &output, &setpoint, 2, 5, 1, DIRECT);
	pid.SetSampleTime(period);
	pid.SetOutputLimits(0,100);
	pid.SetMode(MANUAL);
}

void PIDUpdater::enable() {
	enabled = true;
	pid.SetMode(AUTOMATIC);
}

void PIDUpdater::disable() {
	enabled = false;
	pid.SetMode(MANUAL);
}

void PIDUpdater::brewSetPoint() {
	setpoint = BREW_TEMP;
}

void PIDUpdater::steamSetPoint() {
	setpoint = STEAM_TEMP;
}


void PIDUpdater::setup() {
}

void PIDUpdater::run(Scheduler* scheduler) {
  scheduler->schedule(this, period);

  if(enabled) {
	  input = temp->lastTemp()->boiler;
	  pid.Compute();
	  
	  Serial.print("P|");
	  Serial.print(setpoint);
	  Serial.print("|");
		Serial.print(input);
	  Serial.print("|");
		Serial.print(output);
	  Serial.print("|");
	  Serial.print(pid.GetKp());
	  Serial.print("|");
	  Serial.print(pid.GetKi());
	  Serial.print("|");
	  Serial.println(pid.GetKd());

	  ssr->Out(output, 0);
	}
}