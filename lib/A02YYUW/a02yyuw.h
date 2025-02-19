#ifndef A02YYUW_H
#define A02YYUW_H

#include "Arduino.h"
extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

#define us_get_value_period pdMS_TO_TICKS(100)


class a02yyuw {

    private:
        static boolean _DEBUG;
        static TimerHandle_t _getValueTimer;
        static SemaphoreHandle_t shared_var_mutex;
        static int distance;
        static uint8_t data[128];

        static void getValue();
        static void stopTimer();
        static void startTimer();
    
    public:
        static void init(u_int16_t rxPin, u_int16_t txPin, uint32_t bound_rate, boolean debug = false) {    
            _DEBUG = debug;
            distance = 0;
            _getValueTimer = xTimerCreate(PSTR("a02yyuwTimer"), us_get_value_period, pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(getValue));
            shared_var_mutex = xSemaphoreCreateMutex(); // Create the mutex
            Serial2.begin(bound_rate, SERIAL_8N1, rxPin, txPin);
            getValue();
        }

        static int getDistance() {
            int _dist = 0;
            // Try to take the mutex and wait indefinitely if needed
            if (shared_var_mutex != NULL) {
                if (xSemaphoreTake(shared_var_mutex, portMAX_DELAY) == pdTRUE) { 
                    _dist = distance;
                    xSemaphoreGive(shared_var_mutex);  // After accessing the shared resource give the mutex and allow other processes to access it
                }
            } else _dist = distance;
            return _dist;
        }

        static void setDistance(int _distance) {
            if (shared_var_mutex != NULL) {
                // Try to take the mutex and wait indefinitely if needed
                if (xSemaphoreTake(shared_var_mutex, portMAX_DELAY) == pdTRUE) { 
                    distance = _distance;
                    xSemaphoreGive(shared_var_mutex);  // After accessing the shared resource give the mutex and allow other processes to access it
                }                
            } else distance = _distance;
        }
    
};

#endif