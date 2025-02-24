#ifndef ELECTRICITYCONSUMPTION_H
#define ELECTRICITYCONSUMPTION_H

#include "Arduino.h"
#include <Logger.h>
#include <INA3221.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

extern Logger logger;
#define us_get_electricity_period pdMS_TO_TICKS(100)

struct Electricity {
    float curr;
    float volt;
    float watt;
};

class ElectricityConsumption {

    private:
        static String getText() {
            String s = "";
            if (xSemaphoreTake(shared_var_mutex, portMAX_DELAY) == pdTRUE) { 
                for (uint8_t i = 0; i < 3; i++) {
                    char buffer[50];
                    int len = sprintf(buffer, "A%d: Current:%5.2fA Voltage:%5.2fV power:%5.2fW", i+1, electricityValue[i].curr, electricityValue[i].volt, electricityValue[i].watt);
                    s += buffer;
                    s += i < 2 ? "\n":"";
                }
                xSemaphoreGive(shared_var_mutex);  // After accessing the shared resource give the mutex and allow other processes to access it
            }
            return s;
        };

        static INA3221* ina_0; 
        static TimerHandle_t _getValueTimer;
        static SemaphoreHandle_t shared_var_mutex;
        static Electricity electricityValue[3];


        static void stopTimer();
        static void startTimer();
        static void getValue();

    public:
        static void init(ina3221_addr_t addr) {
            ina_0 = new INA3221(addr);
            ina_0->begin(&Wire);
            ina_0->reset();
            ina_0->setShuntRes(100, 100, 100);
            _getValueTimer = xTimerCreate(PSTR("ElectricityTimer"), us_get_electricity_period, pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(getValue));
            shared_var_mutex = xSemaphoreCreateMutex(); // Create the mutex
            getValue();
        }

        static void consumeElectricity() {
            logger.log(getText().c_str());
        }

        static void electricityConsumption(AsyncWebServerRequest *request){
            String status = "Electricity Consumption is\n";
            status += getText();
            request->send(200, "text/plain", status);
        }

};

#endif