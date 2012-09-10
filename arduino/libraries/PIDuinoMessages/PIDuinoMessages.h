#ifndef PIDuinoMessages_h
#define PIDuinoMessages_h

enum PIDuinoMessageType {
  OptoStatusOut,
  TempStatusOut
};

class OptoStatusMessage
{
  public:
    static void print(OptoStatus *);
};

#endif