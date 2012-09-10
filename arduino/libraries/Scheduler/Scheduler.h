#ifndef SCHEDULER_H
#define SCHEDULER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define QUEUE_MAX 32
#define SCHEDULE_MAX 32

#define INIT_TIMER_COUNT 6
#define RESET_TIMER2 TCNT2 = INIT_TIMER_COUNT

class Scheduler;
class ITask;

typedef struct ScheduleItemStruct
{
  ITask* task;
  unsigned long milliseconds;
} ScheduleItem;

class ITask
{
  public:
    virtual void setup() = 0;
    virtual void run(Scheduler* scheduler) = 0;    
};

class Scheduler
{
  public:
    Scheduler();
    void setup();
    void processMessages();
    
    void queue(ITask* task);
    void clearQueue();
    
    void schedule(ITask*, int); 
    void clearSchedule();
 
  private:
    ITask* _taskQueue[QUEUE_MAX];
    ScheduleItem _taskSchedule[SCHEDULE_MAX];
};

#endif