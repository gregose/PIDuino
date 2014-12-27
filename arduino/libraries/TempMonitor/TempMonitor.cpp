#include "TempMonitor.h"

// Todo implement settings here
TempMonitor::TempMonitor() {
  adc = cADC( A_ADC ); // MCP3424
  amb = ambSensor( A_AMB ); // MCP9800

  temp_samples = (TempSamples){0,0,0,0};
  temp_status = (TempStatus){0,0,0};
  tc = TC_TYPE(); 
}

void TempMonitor::setup() {
  amb.init(AMB_FILTER);
  adc.setCal( CAL_GAIN, UV_OFFSET );
  amb.setOffset( AMB_OFFSET );
  convert_time = max(adc.getConvTime(), amb.getConvTime());
}

uint32_t TempMonitor::convertTime() {
  return convert_time;
}


// Returns min ms to delay
uint32_t TempMonitor::convert(int channel){
  adc.nextConversion(channel);
  amb.nextConversion();
  return convertTime();
}

void TempMonitor::sample(int channel) {
  int32_t v;
  float temp;

  //Serial.print("Sampling - ");
  //Serial.println(channel);

  // Read sensor values
  amb.readSensor(); // retrieve value from ambient temp register
  v = adc.readuV(); // retrieve microvolt sample from MCP3424

  //Serial.print("  Ambient: ");
  //Serial.println(amb.getAmbF());

  //Serial.print("  ADC: ");


  // Convert with ambient temp and to F

#ifdef TEMP_UNIT_F
  temp = tc.Temp_F( 0.001 * v, amb.getAmbF() ); 
#else
  temp = tc.Temp_C( 0.001 * v, amb.getAmbC() );
#endif

  //Serial.println(C_TO_F(tempC));
  //v = round( C_TO_F( tempC ) / D_MULT ); // store results as integers
  
  // Increase sample count
  temp_samples.samples++;

#ifdef TEMP_UNIT_F
  temp_samples.ambient += amb.getAmbF();
#else
  temp_samples.ambient += amb.getAmbC();
#endif

  switch (channel) {
    case BOILER: 
      temp_samples.boiler += temp;
      break;
    case BREWGROUP:
      temp_samples.brewgroup += temp;
      break;
  }
}

void TempMonitor::resetSamples() {
  temp_samples = (TempSamples){0,0,0,0};
}

TempStatus* TempMonitor::status() {
  if(temp_samples.samples < 1)
    return &temp_status;

  temp_status = (TempStatus){
    (temp_samples.ambient) / (temp_samples.samples),
    (temp_samples.boiler) / (temp_samples.samples / 2),
    (temp_samples.brewgroup) / (temp_samples.samples / 2) 
  };

  resetSamples();
  return &temp_status;
}