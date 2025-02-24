#include "electricityConsumption.h"

INA3221* ElectricityConsumption::ina_0 = NULL; 
TimerHandle_t ElectricityConsumption::_getValueTimer = NULL;
SemaphoreHandle_t ElectricityConsumption::shared_var_mutex = NULL;
Electricity ElectricityConsumption::electricityValue[3];

#include "a02yyuw.h"

#include <Logger.h>

extern Logger logger;

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}



void ElectricityConsumption::getValue()
{
    if (shared_var_mutex != NULL) {
        if (xSemaphoreTake(shared_var_mutex, portMAX_DELAY) == pdTRUE) { 
            for (uint8_t i = 0; i < 3; i++) {
                electricityValue[i].curr = ina_0->getCurrent((ina3221_ch_t)i);
                electricityValue[i].volt = ina_0->getVoltage((ina3221_ch_t)i);
                electricityValue[i].watt = electricityValue[i].curr*electricityValue[i].volt;
            }
            xSemaphoreGive(shared_var_mutex);  // After accessing the shared resource give the mutex and allow other processes to access it
        }
    }
    startTimer();
}

void ElectricityConsumption::stopTimer()
{
    if (xTimerIsTimerActive(_getValueTimer) == pdTRUE) {
        xTimerStop(_getValueTimer, 0);
    }
}

void ElectricityConsumption::startTimer()
{
    if (xTimerIsTimerActive(_getValueTimer) == pdFALSE) {
        xTimerStart(_getValueTimer, 0);
    }
}
