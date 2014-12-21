#ifndef PIDuinoTasks_Blinker_h
#define PIDuinoTasks_Blinker_h


class Blinker : public ITask
{
  public:
    void setup();
    void run(Scheduler*);
    Blinker(int, int);
    
  private:
    int _pin;
    int _period;
    int _state;
};

#endif