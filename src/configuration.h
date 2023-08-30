#pragma once
#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#define nameboard "aquarium-cleaner"
#define isPrint2Serial 
#define debug
#define sizeOfLog 128
#define msgLength 64

#define pumpInPin  13
#define pumpInLedOnPin  4
#define pumpInLedOffPin  2

#define pumpOutPin 12
#define pumpOutLedOnPin  18
#define pumpOutLedOffPin  19

#define minLevel 1000
#define maxLevel 1300

#define normalLevel (minLevel + maxLevel)/2

#define deviationWater 100

#define WDT_TIMEOUT 5                   // WDT Timeout in seconds

#define waterSensorPowerPin  15 // ESP32 pin GPIO17 connected to sensor's VCC pin
#define waterSensorSignalPin 34 // ESP32 pin GPIO36 (ADC0) connected to sensor's signal pin



#endif

