#include "PIDUpdater.h"

PIDUpdater::PIDUpdater(int period_in, PWM16* ssr_in, TempUpdater* temp_in, PIDPot* pidpot_in) {
  float Ku = 6.0; // Ultimate gain
  float Tu = 173.0; // Period time in seconds


  // Classic Ziegler-Nichols
  Kp = 0.60 * Ku;
  Ki = (2.0 * Kp) / Tu;
  Kd = (Kp * Tu) / 8;

  // some overshoot
  //Kp = 0.33 * Ku;
  //Ki = (2.0 * Kp) / Tu;
  //Kd = (Kp * Tu) / 3;

  // no overshoot
  //Kp = 0.20 * Ku;
  //Ki = (2.0 * Kp) / Tu;
  //Kd = (Kp * Tu) / 3;

  temp = temp_in;
  ssr = ssr_in;
  period = period_in;
  pidpot = pidpot_in;

  setpoint = BREW_TEMP;

  disable();
                                        //k: P  I  D
  //pid = PID(&input, &output, &setpoint, 2, 5, 1, DIRECT);

  /* Tuned PID params:
    https://controls.engin.umich.edu/wiki/index.php/PIDTuningClassical
    Ziegler-Nichols params
    Ku = 8

    Peaks: 14:48.9, 12:25.6
    Diff: 2:23.3

    Pu = 143.3

    Kc = Ku/1.7 = 4.7
    Ti = Pu/2 = 71.7
    Td = Pu/8 = 17.9

    http://newton.ex.ac.uk/teaching/CDHW/Feedback/Setup-PID.html
      It should yield a system that is slightly underdamped; if a less "aggressive" response is 
      desired try reducing P to half the values listed.
  */

  //pid = PID(&input, &output, &setpoint, 6, 2, 1, DIRECT);

  //pid.SetSampleTime(period);
  //pid.SetOutputLimits(0,100);
  //pid.SetMode(MANUAL);
}

void PIDUpdater::enable() {
  enabled = true;
  //pid.SetMode(AUTOMATIC);
}

void PIDUpdater::disable() {
  enabled = false;

  // Reset PID calculations
  previous_time = 0;
  previous_error = 0;
  integral = 0;
  //pid.SetMode(MANUAL);
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
  
  float error;
  float derivative;
  float output;
  float dt;
  float measured_value;

  if(enabled) {
    if(previous_time == 0) {
      previous_time = millis(); // use exact time in case scheduler slows down
      scheduler->schedule(this, period);
      return;
    }

    dt = (millis() - previous_time) / 1000;

    measured_value = temp->lastTemp()->boiler;

    error = setpoint - measured_value;
    integral += error * dt;
    derivative = (error - previous_error) / dt;
    output = (Kp * error) + (Ki * integral) + (Kd * derivative);
    previous_error = error;

    // set output limits
    if (output > 100) output = 100;
    if (output < 0) output = 0;

    Serial.print("P|");
    Serial.print(dt);
    Serial.print("|");
    Serial.print(setpoint);
    Serial.print("|");
    Serial.print(measured_value);
    Serial.print("|");
    Serial.print(output);
    Serial.print("|");
    Serial.print(error);
    Serial.print("|");
    Serial.print(integral);
    Serial.print("|");
    Serial.print(derivative);
    Serial.print("|");
    Serial.print(Kp);
    Serial.print("|");
    Serial.print(Ki);
    Serial.print("|");
    Serial.println(Kd);


    ssr->Out(output, 0); // Set OT1 to output

    previous_time = millis();
  }

  scheduler->schedule(this, period);
}