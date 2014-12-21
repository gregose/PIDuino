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
  
  // Prevent integral windup from intial machine warmup
  // From http://playground.arduino.cc/Main/BarebonesPIDForEspresso#pid
  // http://en.wikipedia.org/wiki/Integral_windup

  // Conditional integration http://protuner.com/app6.pdf, https://controls.engin.umich.edu/wiki/index.php/PIDDownsides#Windup
  integral_cutoff_error = 10;
  integral_max = 200;

  temp = temp_in; // Temp status
  ssr = ssr_in; // SSR, OT1 - boiler
  period = period_in; // Update period
  pidpot = pidpot_in; // Manual control if needed

  setpoint = BREW_TEMP;

  disable(0);
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
}

void PIDUpdater::enable() {
  enabled = true;
}

void PIDUpdater::disable(int duty) {
  enabled = false;

  // Reset PID calculations
  previous_time = 0;
  previous_error = 0;
  integral = 0;

  // Set SSR duty
  if(duty == -1) // maintain
    ssr->Out(output, 0);
  else // set to passed value
    ssr->Out(duty, 0); // Set OT1 to output
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
  float dt;
  float measured_value;
  float iterm;

  if(enabled) {

    if(previous_time == 0) {
      previous_time = millis(); // use exact time in case scheduler slows down
      scheduler->schedule(this, period);
      return;
    }

    dt = (millis() - previous_time) / 1000;

    measured_value = temp->lastTemp()->boiler;

    error = setpoint - measured_value;

    
    derivative = (error - previous_error) / dt;

    if(error > integral_cutoff_error) { // disable integration if large error
      output = (Kp * error) + (Kd * derivative);
    } else {
      integral += error * dt;
      // windup protection
      if(integral > integral_max)
        integral = integral_max;
      else if(integral < -integral_max)
        integral = -integral_max;

      output = (Kp * error) + (Ki * integral) + (Kd * derivative);
    }
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