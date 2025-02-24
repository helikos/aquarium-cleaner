#pragma once
#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#define nameboard "aquarium-cleaner"
#define _debug


#define minLevel 50
#define maxLevel 60

#define normalLevel (minLevel + maxLevel)/2

#define deviationWater 5

#define WDT_TIMEOUT 5                   // WDT Timeout in seconds


#define sonicTX  17
#define sonicRX  16
#define sonic_baund_rate 9600


#define pumpInLedOnPin  2
#define pumpInLedOffPin  4
#define pumpInIn1Pin  25
#define pumpInIn2Pin  33
#define pumpInEnPin   32

#define pumpOutLedOnPin  18
#define pumpOutLedOffPin  19
#define pumpOutIn1Pin  14
#define pumpOutIn2Pin  27
#define pumpOutEnPin   26


#define _consumeElectricityPeriod pdMS_TO_TICKS(1000)


#endif

