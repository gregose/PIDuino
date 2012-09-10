#ifndef PIDuinoTasks_TempUpdater_h
#define PIDuinoTasks_TempUpdater_h

class TempUpdater : public ITask
{
  public:
    void setup();
    void run(Scheduler*);
    TempUpdater(int);
    
  private:
    int _period;
    TempMonitor _temp_mon;
};

#endif