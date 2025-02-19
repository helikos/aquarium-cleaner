#pragma once
#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#define nameboard "aquarium-cleaner"
#define isPrint2Serial 
#define _debug
#define sizeOfLog 128
#define msgLength 64

#define pumpInPin  13
#define pumpInLedOnPin  4
#define pumpInLedOffPin  2

#define pumpOutPin 12
#define pumpOutLedOnPin  18
#define pumpOutLedOffPin  19

#define minLevel 1500
#define maxLevel 1700

#define normalLevel (minLevel + maxLevel)/2

#define deviationWater 50

#define WDT_TIMEOUT 5                   // WDT Timeout in seconds

#define waterSensorPowerPin  15 // ESP32 pin GPIO17 connected to sensor's VCC pin
#define waterSensorSignalPin 34 // ESP32 pin GPIO36 (ADC0) connected to sensor's signal pin

const int trigPin = 17;
const int echoPin = 16;

#define sonicTX  17
#define sonicRX  16
#define sonic_baund_rate 9600


//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

#define motor1In1  25
#define motor1In2  33
#define motor1En   32

#define motor2In1  14
#define motor2In2  27
#define motor2En   26

#define _consumeElectricityPeriod pdMS_TO_TICKS(1000)

#define us_get_value_period pdMS_TO_TICKS(100)

#endif

