#ifndef A02YYUW_H
#define A02YYUW_H

#include "Arduino.h"
extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}


class a02yyuw {
    
    public:
        static void init(u_int16_t rxPin, u_int16_t txPin, uint32_t bound_rate, boolean debug = false);
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
        };

        static void setDistance(int _distance) {
            if (shared_var_mutex != NULL) {
                // Try to take the mutex and wait indefinitely if needed
                if (xSemaphoreTake(shared_var_mutex, portMAX_DELAY) == pdTRUE) { 
                    distance = _distance;
                    xSemaphoreGive(shared_var_mutex);  // After accessing the shared resource give the mutex and allow other processes to access it
                }                
            } else distance = _distance;
        };
    
    private:
        static u_int16_t _rxPin;
        static u_int16_t _txPin;
        static uint32_t _bound_rate;
        static boolean _DEBUG;
        static TimerHandle_t _getValueTimer;
        static SemaphoreHandle_t shared_var_mutex;
        static int distance;

        static void getValue();
        static void stopTimer();
        static void startTimer();


};

#endif