#pragma once
#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#define nameboard "aquarium-cleaner"
#define isPrint2Serial 
#define debug
#define sizeOfLog 128
#define msgLength 64

#define pumpInPin  13
#define pumpInLedOnPin  2
#define pumpInLedOffPin  4

#define pumpOutPin 12
#define pumpOutLedOnPin  19
#define pumpOutLedOffPin  18


#define rangeWater 80
#define deviationWater 2
#define deviationWater4Motor 2

#define WDT_TIMEOUT 5                   // WDT Timeout in seconds

#define POWER_PIN  17 // ESP32 pin GPIO17 connected to sensor's VCC pin
#define SIGNAL_PIN 36 // ESP32 pin GPIO36 (ADC0) connected to sensor's signal pin
#define SENSOR_MIN 0
#define SENSOR_MAX 521


#endif

