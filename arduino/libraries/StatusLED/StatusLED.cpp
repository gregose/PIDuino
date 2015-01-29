#include "StatusLED.h"

StatusLED::StatusLED(uint8_t r_pin_in, uint8_t g_pin_in, uint8_t b_pin_in) {
  r_pin = r_pin_in;
  g_pin = g_pin_in;
  b_pin = b_pin_in;

  pinMode(r_pin, OUTPUT);
  pinMode(g_pin, OUTPUT);
  pinMode(b_pin, OUTPUT);

  r_v= 0;
  g_v = 0;
  b_v = 0;
  a_v = 100;
}

void StatusLED::red() {
  r(255);
  g(0);
  b(0);
  update();
}

void StatusLED::green() {
  r(0);
  g(255);
  b(0);
  update();
}

void StatusLED::blue() {
  r(0);
  g(0);
  b(255);
  update();
}

void StatusLED::orange() {
  r(255);
  g(100);
  b(0);
  update();
}

void StatusLED::yellow() {
  r(255);
  g(255);
  b(0);
  update();
}

void StatusLED::purple() {
  r(160);
  g(32);
  b(240);
  update();
}

void StatusLED::off() {
  alpha(0);
}

void StatusLED::on() {
  alpha(1);
}

void StatusLED::update() {
  analogWrite(r_pin, r_v * a_v);
  analogWrite(g_pin, g_v * a_v);
  analogWrite(b_pin, b_v * a_v);
}

void StatusLED::alpha(float v) {
  a_v = v;
  update();
}

void StatusLED::r(int v) {
  r_v = v;
}

void StatusLED::g(int v) {
  g_v = v;
}

void StatusLED::b(int v) {
  b_v = v;
}
