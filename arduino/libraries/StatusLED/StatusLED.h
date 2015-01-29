#ifndef STATUS_LED_H
#define STATUS_LED_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class StatusLED {
public:
  StatusLED(uint8_t, uint8_t, const uint8_t);
  void red();
  void green();
  void blue();
  void orange();
  void yellow();
  void purple();
  void off();
  void on();
  void alpha(float);
private:
  uint8_t r_pin;
  uint8_t g_pin;
  uint8_t b_pin;
  int r_v;
  int g_v;
  int b_v;
  float a_v;
  void update();
  void r(int);
  void g(int);
  void b(int);
};

#endif
