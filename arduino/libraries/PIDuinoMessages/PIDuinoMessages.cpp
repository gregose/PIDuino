#include <PIDuinoMessages.h>

void OptoStatusMessage::print(OptoStatus* opto_status) {
  Serial.printf("%d:%d|%d|%d|%d", PIDuinoMessageType.OptoStatusOut, opto_status->ch0, opto_status->ch1, opto_status->ch2, opto_status->ch3);
}