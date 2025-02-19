#include "pump.h"

boolean Pump::_DEBUG = false;
L298N* Pump::inPumn;
L298N* Pump::outPumn;
u_int8_t Pump::inLedOnPin = 0;
u_int8_t Pump::inLedOffPin = 0;
u_int8_t Pump::outLedOnPin = 0;
u_int8_t Pump::outLedOffPin = 0;
boolean Pump::pumnInState = false;
boolean Pump::pumnOutState = false;

