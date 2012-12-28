#ifndef PIDuinoTasks_PIDUpdater_h
#define PIDuinoTasks_PIDUpdater_h

#include "TempUpdater.h"
#include <PWM16.h>
#include <PIDPot.h>
#include <Scheduler.h>

// #define BREW_TEMP 195.00
// #define STEAM_TEMP 290.00

#define BREW_TEMP 150.00
#define STEAM_TEMP 190.00


class PIDUpdater : public ITask
{
  public:
    void setup();
    void run(Scheduler*);
    PIDUpdater(int, PWM16*, TempUpdater*, PIDPot*);
    void enable();
    void disable(int);
    void brewSetPoint();
    void steamSetPoint();
    
  private:
    int period;
    TempUpdater* temp;
    PWM16* ssr;
    PIDPot* pidpot;

    float setpoint;
    bool enabled;

    float previous_error;
    float previous_time;
    float integral;
    float output;
    float integral_max;
    float integral_cutoff_error;

    float Kp;
    float Ki;
    float Kd;

};

#endif