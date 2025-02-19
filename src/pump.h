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

        static boolean _DEBUG;
        static L298N* inPumn;
        static L298N* outPumn;
        static u_int8_t inLedOnPin;
        static u_int8_t inLedOffPin;
        static u_int8_t outLedOnPin;
        static u_int8_t outLedOffPin;
        static boolean pumnInState;
        static boolean pumnOutState;

        static L298N* initPumn(u_int8_t _PumpIn1Pin, u_int8_t _PumpIn2Pin
                       ,u_int8_t _PumpEnPin, u_int8_t _PumpLedOnPin, u_int8_t _PumpLedOffPin) {
            
            auto pumn = new L298N(_PumpIn1Pin, _PumpIn2Pin, _PumpEnPin, PWM_CHB, _DEBUG);
            pumn->setSpeed(100); 
            pumn->begin();
            pumn->brake(); 

            pinMode(_PumpLedOnPin, OUTPUT);
            pinMode(_PumpLedOffPin, OUTPUT);
            digitalWrite(_PumpLedOnPin, LOW);
            digitalWrite(_PumpLedOffPin, LOW);
            return pumn;
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

    public:
        static void init(u_int8_t _PumpInIn1Pin, u_int8_t _PumpInIn2Pin, u_int8_t _PumpInEnPin, u_int8_t _PumpInLedOnPin, u_int8_t _PumpInLedOffPin
                    ,u_int8_t _PumpOutIn1Pin, u_int8_t _PumpOutIn2Pin, u_int8_t _PumpOutEnPin, u_int8_t _PumpOutLedOnPin, u_int8_t _PumpOutLedOffPin
                    ,boolean debug = false) {    
            
            inLedOnPin = _PumpInLedOnPin;
            inLedOffPin = _PumpInLedOffPin;
            outLedOnPin = _PumpOutLedOnPin;
            outLedOffPin = _PumpOutLedOffPin;
            _DEBUG = debug;
            pumnInState = false;
            pumnOutState = false;


            inPumn = initPumn(_PumpInIn1Pin, _PumpInIn2Pin, _PumpInEnPin, _PumpInLedOnPin, _PumpInLedOffPin);
            outPumn = initPumn(_PumpOutIn1Pin, _PumpOutIn2Pin, _PumpOutEnPin, _PumpOutLedOnPin, _PumpOutLedOffPin);
        }

        static void processing() {
            int distance = sonic_sensor.getDistance();
            logger.log(PSTR("Distance in %d mm"), distance);
            if (distance == 0){
                logger.log(PSTR("PumnIn: Off, PumnOut: Off"));
                pumnInOff();
                pumnOutOff();
            } else if (abs(distance - normalLevel) <= deviationWater) {
                logger.log(PSTR("PumnIn: On, PumnOut: On"));
                pumnInOn();
                pumnOutOn();
            } else if (distance > (normalLevel + deviationWater)) {
                logger.log(PSTR("PumnIn: On, PumnOut: Off"));
                pumnInOn();
                pumnOutOff();
            }
            else if (distance < (normalLevel - deviationWater)) {
                logger.log(PSTR("PumnIn: Off, PumnOut: On"));
                pumnInOff();
                pumnOutOn();
            } else {
                logger.log(PSTR("PumnIn: Off, PumnOut: Off"));
                pumnInOff();
                pumnOutOff();
            }
        }
};

#endif