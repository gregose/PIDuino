#ifndef PIDuinoTasks_OptoStatusUpdater_h
#define PIDuinoTasks_OptoStatusUpdater_h

#include "PIDUpdater.h"
#include <OptoIn.h>
#include <Scheduler.h>

class OptoStatusUpdater : public ITask
{
  public:
    void setup();
    void run(Scheduler*);
    OptoStatusUpdater(int, PIDUpdater*);
    
  private:
    int period;
    OptoIn opto_in;
    PIDUpdater* pid;
};

#endif