#ifndef PIDuinoTasks_PIDUpdater_h
#define PIDuinoTasks_PIDUpdater_h

#include "TempUpdater.h"
#include <PWM16.h>
#include <PID.h>
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
    PIDUpdater(int, PWM16*, TempUpdater*);
    void enable();
    void disable();
    void brewSetPoint();
    void steamSetPoint();
    
  private:
    int period;
    TempUpdater* temp;
    PWM16* ssr;
    PID pid;
    double input;
    double output;
    double setpoint;
    bool enabled;
};

#endif