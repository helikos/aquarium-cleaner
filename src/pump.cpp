#include "pump.h"

bool Pump::_DEBUG = false;
L298N* Pump::inPumn;
L298N* Pump::outPumn;
u_int8_t Pump::inLedOnPin = 0;
u_int8_t Pump::inLedOffPin = 0;
u_int8_t Pump::outLedOnPin = 0;
u_int8_t Pump::outLedOffPin = 0;
u_int8_t Pump::solenoidPin = 0;
bool Pump::pumnInState = false;
bool Pump::pumnOutState = false;
bool Pump::solenoidState = false;
