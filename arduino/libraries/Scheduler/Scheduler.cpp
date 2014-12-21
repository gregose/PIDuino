//// scheduler ////
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License Version 2
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You will find the latest version of this code at the following address:
// http://github.com/pchretien
//
// You can contact me at the following email address:
// philippe.chretien at gmail.com


//////////////////////////////////////////////////////////////
// Scheduler
//////////////////////////////////////////////////////////////

#include "Scheduler.h"

unsigned long __ulCounter = 0;

// User timer4 for atmega32u4
// http://harizanov.com/2013/04/crazy-high-frequency-pwm-with-atmega32u4/
// http://docs.ros.org/hydro/api/segbot_firmware/html/NewPing_8cpp_source.html

ISR(TIMER4_OVF_vect) {
  TCNT4 = 0;
  __ulCounter++;
};

Scheduler::Scheduler()
{
  clearQueue();
  clearSchedule();
}

void Scheduler::setup()
{
  // Disable timer4 int
  TIMSK4 = 0;
  // Clear
  TCCR4A = TCCR4C = TCCR4D = TCCR4E = 0;
  // Set Timer4 prescaler to 64 (4uS/count, 4uS-1020uS range).
  TCCR4B = (1<<CS42) | (1<<CS41) | (1<<CS40) | (1<<PSR4);
  TIFR4 = (1<<TOV4);
  TCNT4 = 0;

  OCR4C = 249;         // Every count is 4uS, so 1ms = 250 counts - 1.
  TIMSK4 = (1<<TOIE4); // Enable Timer4 interrupt.

  sei();
}

void Scheduler::processMessages()
{
  // Check the schedule
  for( int i=0; i<SCHEDULE_MAX; i++)
  {
    if(_taskSchedule[i].task &&
       _taskSchedule[i].milliseconds <= __ulCounter)
    {
      queue(_taskSchedule[i].task);
      _taskSchedule[i].task = 0;
    }
  }

  // Clear the queue
  for( int i=0; i<QUEUE_MAX; i++)
  {
    if(_taskQueue[i] )
    {
      _taskQueue[i]->run(this);
      _taskQueue[i] = 0;
    }
  }
}

void Scheduler::queue(ITask* task)
{
  for( int i=0; i<QUEUE_MAX; i++)
  {
    if(!_taskQueue[i] )
    {
      _taskQueue[i] = task;
      return;
    }
  }
}

void Scheduler::clearQueue()
{
  for( int i=0; i<QUEUE_MAX; i++)
    _taskQueue[i] = 0;
}

void Scheduler::schedule(ITask* task, int milliseconds)
{
  for( int i=0; i<SCHEDULE_MAX; i++)
  {
    if(!_taskSchedule[i].task)
    {
      _taskSchedule[i].task = task;
      _taskSchedule[i].milliseconds = __ulCounter + milliseconds;
      return;
    }
  }
}

void Scheduler::clearSchedule()
{
  for( int i=0; i<SCHEDULE_MAX; i++)
    _taskSchedule[i].task = 0;
}
