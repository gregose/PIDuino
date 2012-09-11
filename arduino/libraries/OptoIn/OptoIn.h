#ifndef OPTOIN_H
#define OPTOIN_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define OPTOIN_CH0_PIN 4
#define OPTOIN_CH1_PIN 5
#define OPTOIN_CH2_PIN 6
#define OPTOIN_CH3_PIN 7

struct OptoStatus
{
  boolean ch0;
  boolean ch1;
  boolean ch2;
  boolean ch3;
};

class OptoIn {
  public:
    OptoIn();
    void Setup();
    boolean Update();
    OptoStatus* Status();
  private:
    OptoStatus _status;
    OptoStatus _previous_status;  
};

#endif