#ifndef PUMP_H
#define PUMP_H

#include "Arduino.h"
#include "L298N.h"
#include "a02yyuw.h"
#include "configuration.h"
#include <Logger.h>

extern Logger logger;
extern a02yyuw sonic_sensor;

class Pump{

    private:

        static bool _DEBUG;
        static L298N* inPumn;
        static L298N* outPumn;
        static u_int8_t inLedOnPin;
        static u_int8_t inLedOffPin;
        static u_int8_t outLedOnPin;
        static u_int8_t outLedOffPin;
        static u_int8_t solenoidPin;
        static bool pumnInState;
        static bool pumnOutState;
        static bool solenoidState;

        static L298N* initPumn(u_int8_t _PumpIn1Pin, u_int8_t _PumpIn2Pin
                       ,u_int8_t _PumpEnPin, u_int8_t _PumpLedOnPin, u_int8_t _PumpLedOffPin, u_int8_t _pumpPWMCnl, u_int8_t speed) {
            
            auto pumn = new L298N(_PumpIn1Pin, _PumpIn2Pin, _PumpEnPin, _pumpPWMCnl, _DEBUG);
            pumn->begin();
            pumn->setSpeed(speed); 
            pumn->forward();
            pumn->brake(); 

            pinMode(_PumpLedOnPin, OUTPUT);
            pinMode(_PumpLedOffPin, OUTPUT);
            digitalWrite(_PumpLedOnPin, LOW);
            digitalWrite(_PumpLedOffPin, LOW);
            return pumn;
        }

        static void initSolenoid(u_int8_t _SolenoidPin) {
            pinMode(_SolenoidPin, OUTPUT);
        }

        static void pumnOn(L298N* pumn, u_int8_t ledOnPin, u_int8_t ledOffPin){
            if (pumn != NULL) {
                pumn->forward();
            }
            digitalWrite(ledOnPin, HIGH);
            digitalWrite(ledOffPin, LOW);
        }

        static void pumnOff(L298N* pumn, u_int8_t ledOnPin, u_int8_t ledOffPin){
            if (pumn != NULL) {
                pumn->brake();
            }
            digitalWrite(ledOnPin, LOW);
            digitalWrite(ledOffPin, HIGH);
        }

        static void pumnInOn(){
            if (!pumnInState) {
                if (_DEBUG)  logger.log(PSTR("Pumn In On"));
                pumnOn(inPumn, inLedOnPin, inLedOffPin);
                pumnInState = true;
            }
        }

        static void pumnInOff(){
            if (pumnInState) {
                if (_DEBUG)  logger.log(PSTR("Pumn In Off"));
                pumnOff(inPumn, inLedOnPin, inLedOffPin);
                pumnInState = false;
            }
        }

        static void pumnOutOn(){
            if (!pumnOutState) {
                if (_DEBUG)  logger.log(PSTR("Pumn Out On"));
                pumnOn(outPumn, outLedOnPin, outLedOffPin);
                pumnOutState = true;
            }
        }

        static void pumnOutOff(){
            if (pumnOutState) {
                if (_DEBUG)  logger.log(PSTR("Pumn Out Off"));
                pumnOff(outPumn, outLedOnPin, outLedOffPin);
                pumnOutState = false;
            }
        }

        static void solenoidOn(){
            if (!solenoidState) {
                if (_DEBUG)  logger.log(PSTR("Solenoid On"));
                digitalWrite(solenoidPin, HIGH);
                solenoidState = true;
            }
        }

        static void solenoidOff(){
            if (solenoidState) {
                if (_DEBUG)  logger.log(PSTR("Solenoid Off"));
                digitalWrite(solenoidPin, LOW);
                solenoidState = false;
            }
        }

    public:
        static void init(u_int8_t _PumpInIn1Pin, u_int8_t _PumpInIn2Pin, u_int8_t _PumpInEnPin, u_int8_t _PumpInLedOnPin, u_int8_t _PumpInLedOffPin, u_int8_t _pumpInPWMCnl
                    ,u_int8_t _PumpOutIn1Pin, u_int8_t _PumpOutIn2Pin, u_int8_t _PumpOutEnPin, u_int8_t _PumpOutLedOnPin, u_int8_t _PumpOutLedOffPin, u_int8_t _pumpOutPWMCnl
                    ,u_int8_t _SolenoidPin, bool debug = false) {    
            
            inLedOnPin = _PumpInLedOnPin;
            inLedOffPin = _PumpInLedOffPin;
            outLedOnPin = _PumpOutLedOnPin;
            outLedOffPin = _PumpOutLedOffPin;
            solenoidPin = _SolenoidPin;
            _DEBUG = debug;
            pumnInState = true;
            pumnOutState = true;
            solenoidState = true;
            inPumn = initPumn(_PumpInIn1Pin, _PumpInIn2Pin, _PumpInEnPin, _PumpInLedOnPin, _PumpInLedOffPin, _pumpInPWMCnl, 100);
            outPumn = initPumn(_PumpOutIn1Pin, _PumpOutIn2Pin, _PumpOutEnPin, _PumpOutLedOnPin, _PumpOutLedOffPin, _pumpOutPWMCnl, 10);
            initSolenoid(_SolenoidPin);
            solenoidOff();
            pumnInOff();
            pumnOutOff();
        }

        static void processing() {
            int distance = sonic_sensor.getDistance();
            logger.log(PSTR("Distance in %d mm"), distance);
            if (distance == 0){
                logger.log(PSTR("PumnIn: Off, PumnOut: Off"));
                pumnInOff();
                pumnOutOff();
                solenoidOff();
            } else if (abs(distance - normalLevel) <= deviationWater) {
                logger.log(PSTR("PumnIn: On, PumnOut: On"));
                pumnInOn();
                pumnOutOn();
                solenoidOn();
            } else if (distance > (normalLevel + deviationWater)) {
                logger.log(PSTR("PumnIn: On, PumnOut: Off"));
                pumnInOn();
                pumnOutOff();
                solenoidOn();
            }
            else if (distance < (normalLevel - deviationWater)) {
                logger.log(PSTR("PumnIn: Off, PumnOut: On"));
                pumnInOff();
                pumnOutOn();
            } else {
                logger.log(PSTR("PumnIn: Off, PumnOut: Off"));
                pumnInOff();
                pumnOutOff();
                solenoidOff();
            }
        }

        static void test() {
//            pumnInOn();
            inPumn->setSpeed(100);
            inPumn->forward();
            outPumn->setSpeed(10);
            outPumn->forward();
/*
            for(int i=0; i<=100; i++) {
                debug("Speed %d forward. delay(%d)", i, 100);
                inPumn->setSpeed(i);
                inPumn->forward();
                delay(1000);
              }
*/

        }

};

#endif