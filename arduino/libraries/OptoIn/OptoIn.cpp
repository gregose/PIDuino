#include "OptoIn.h"

OptoIn::OptoIn() {
  _previous_status = (OptoStatus){false, false, false, false};
  _status = (OptoStatus){false, false, false, false};
}

void OptoIn::Setup() {
  pinMode(OPTOIN_CH0_PIN, INPUT);
  pinMode(OPTOIN_CH1_PIN, INPUT);
  pinMode(OPTOIN_CH2_PIN, INPUT);
  pinMode(OPTOIN_CH3_PIN, INPUT);
}

OptoStatus* OptoIn::Status() {
  return &_status;
}

boolean OptoIn::Update() {
  _previous_status.ch0 = _status.ch0;
  _previous_status.ch1 = _status.ch1;
  _previous_status.ch2 = _status.ch2;
  _previous_status.ch3 = _status.ch3;
  
  _status.ch0 = digitalRead(OPTOIN_CH0_PIN) == 0;
  _status.ch1 = digitalRead(OPTOIN_CH1_PIN) == 0;
  _status.ch2 = digitalRead(OPTOIN_CH2_PIN) == 0;
  _status.ch3 = digitalRead(OPTOIN_CH3_PIN) == 0;

  return _previous_status.ch0 != _status.ch0
         || _previous_status.ch1 != _status.ch1
         || _previous_status.ch2 != _status.ch2
         || _previous_status.ch3 != _status.ch3; 
}
