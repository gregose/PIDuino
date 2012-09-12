#ifndef PIDuinoTasks_OptoStatusUpdater_h
#define PIDuinoTasks_OptoStatusUpdater_h

#include <PWM16.h>
#include <OptoIn.h>
#include <Scheduler.h>

class OptoStatusUpdater : public ITask
{
  public:
    void setup();
    void run(Scheduler*);
    OptoStatusUpdater(int, PWM16*);
    
  private:
    int period;
    OptoIn opto_in;
    PWM16* ssr;
};

#endif