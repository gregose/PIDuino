#ifndef PIDuinoTasks_TempUpdater_h
#define PIDuinoTasks_TempUpdater_h

#include <Scheduler.h>
#include <TempMonitor.h>

#define RUN_STATE_IDLE 0
#define RUN_STATE_CONVERT_BOILER 1
#define RUN_STATE_SAMPLE_BOILER 2
#define RUN_STATE_CONVERT_BREWGROUP 3
#define RUN_STATE_SAMPLE_BREWGROUP 4
#define RUN_STATE_READ 5

class TempUpdater : public ITask
{
  public:
    void setup();
    void run(Scheduler*);
    TempUpdater(int);
    
  private:
    void next();
    int period;
    int run_state;
    TempMonitor temp_monitor;
};

#endif