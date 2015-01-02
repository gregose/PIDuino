#include "Settings.h"

Settings::Settings() {

}

uint16_t Settings::update() {
  eeprom.read( 0, (uint8_t*) &_data, sizeof( _data ) );
}

SettingsBlock* Settings::data() {
  return &_data;
}

boolean Settings::available() {
  return _data.PCB[0] == 'T' &&
         _data.PCB[1] == 'C' &&
         _data.PCB[2] == '4';
}

void Settings::print() {
  Serial1.println("---CONFIG---");
  Serial1.println(_data.PCB);
  Serial1.println(_data.version);
  Serial1.println(_data.cal_gain, DEC); // 1.0041999816
  Serial1.println(_data.cal_offset); // 0
  Serial1.println(_data.T_offset); // -0.80
  Serial1.println(_data.K_offset); // -0.80
  Serial1.println("---CONFIG---");
}

void Settings::log() {
  Serial1.print("E|");
  Serial1.print("|");
  Serial1.print(_data.PCB);
  Serial1.print("|");
  Serial1.print(_data.version);
  Serial1.print("|");
  Serial1.print(_data.cal_gain, DEC); // 1.0041999816
  Serial1.print("|");
  Serial1.print(_data.cal_offset); // 0
  Serial1.print("|");
  Serial1.print(_data.T_offset); // -0.80
  Serial1.print("|");
  Serial1.print(_data.K_offset); // -0.80
  Serial1.println();
}
