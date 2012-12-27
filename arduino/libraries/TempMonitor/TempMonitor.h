#ifndef TEMPMONITOR_H
#define TEMPMONITOR_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <cADC.h>
#include <thermocouple.h>

// ambient sensor should be stable, so quick variations are probably noise -- filter heavily
#define AMB_FILTER 70 // 70% filtering on ambient sensor readings

// default values for systems without calibration values stored in EEPROM
#define CAL_GAIN 1.0 // you may substitute a known gain adjustment from calibration
#define UV_OFFSET 38 // you may subsitute a known value for uV offset in ADC
#define AMB_OFFSET -1.8 // you may substitute a known value for amb temp offset (Celsius)

#define TC_TYPE typeT
#define D_MULT 0.001 // multiplier to convert temperatures from int to float

#define BOILER 0
#define BREWGROUP 1 

#define TEMP_UNIT_F 
//#define TEMP_UNIT_C

struct TempSamples
{
  float samples;
  float ambient;
  float boiler;
  float brewgroup;
};

struct TempStatus
{
  float ambient;
  float boiler;
  float brewgroup;
};

class TempMonitor {
  public:
    TempMonitor();
    void setup();

    uint32_t convert(int channel);
    void sample(int channel);

    TempStatus* status();
    void resetSamples();
    uint32_t convertTime();
  private:
    cADC adc;
    ambSensor amb;
    TempStatus temp_status;
    TempSamples temp_samples;
    TC_TYPE tc;
    uint32_t convert_time;
};

#endif