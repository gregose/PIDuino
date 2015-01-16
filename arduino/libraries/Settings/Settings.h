#ifndef SETTINGS_H
#define SETTINGS_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <mcEEPROM.h>

// eeprom calibration data structure for TC4 and additional settings
// for PIDuino
struct SettingsBlock {
  char PCB[40]; // identifying information for the board
  char version[16];
  float cal_gain;  // calibration factor of ADC at 50000 uV
  int16_t cal_offset; // uV, probably small in most cases
  float T_offset; // temperature offset (Celsius) at 0.0C (type T)
  float K_offset; // same for type K
  float brew_setpoint;
  float steam_setpoint;
};

class Settings {
public:
  Settings();
  boolean available();
  uint16_t update();
  uint16_t save();
  void print();
  void log();
  SettingsBlock* data();
private:
  SettingsBlock _data;
  mcEEPROM eeprom;
};

#endif
