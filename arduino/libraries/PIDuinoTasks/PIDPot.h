#ifndef PIDuinoTasks_PIDPot_h
#define PIDuinoTasks_PIDPot_h


class PIDPot : public ITask
{
  public:
    void setup();
    void run(Scheduler*);
    PIDPot(int, int);
	double val();

  private:
    int _pin;
    int _period;
    int _val;
};

#endif