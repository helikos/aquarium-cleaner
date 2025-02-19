#include "a02yyuw.h"

#include <Logger.h>

extern Logger logger;

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}


TimerHandle_t a02yyuw::_getValueTimer = NULL;
SemaphoreHandle_t a02yyuw::shared_var_mutex = NULL;
boolean a02yyuw::_DEBUG;
int a02yyuw::distance;
uint8_t a02yyuw::data[128];


void a02yyuw::getValue()
{
    String s = "";
    int available = Serial2.available();
    if (available >= 4) {
        for (int i = 0; i < available; i++){
            data[i] = Serial2.read();
            if (_DEBUG) {
                s += (char) data[i];
            }
        }
        if (data[0] == 0xFF) { // Check packet start byte
            int sum = (data[0] + data[1] + data[2]) & 0xFF;
            if (sum == data[3]){                         // Validate checksum
                int distance = (data[1] << 8) + data[2]; // Calculate distance
                if (distance > 30) {                     // Ensure valid reading
                if (_DEBUG) logger.log(PSTR("Distance: %d mm"), distance);
                    setDistance(distance);
                }
                else {
                    if (_DEBUG) logger.log(PSTR("Below the lower limit"));
                    setDistance(0);
                }
            }
            else {
                if (_DEBUG) logger.log(PSTR("Checksum error"));
                setDistance(0);
            }
        }
    } else if (_DEBUG) logger.log(PSTR("data not available"));
    startTimer();
}

void a02yyuw::stopTimer()
{
    if (xTimerIsTimerActive(_getValueTimer) == pdTRUE) {
        if (_DEBUG) logger.log(PSTR("a02yyuw::stopTimer"));
        xTimerStart(_getValueTimer, 0);
    }
}

void a02yyuw::startTimer()
{
    if (xTimerIsTimerActive(_getValueTimer) == pdFALSE) {
        if (_DEBUG) logger.log(PSTR("a02yyuw::startTimer"));
        xTimerStart(_getValueTimer, 0);
    }
}
