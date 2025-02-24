#ifndef ELECTRICITYCONSUMPTION_H
#define ELECTRICITYCONSUMPTION_H

#include "Arduino.h"
#include <Logger.h>
#include <INA3221.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

extern Logger logger;

class ElectricityConsumption {

    private:
        static String getText(String s) {
            for (uint8_t i = 0; i < 3; i++) {
                auto curr = ina_0->getCurrent((ina3221_ch_t)i);
                auto volt = ina_0->getVoltage((ina3221_ch_t)i);
                auto watt = curr * volt;
                char buffer[50];
                int len = sprintf(buffer, "A%d: Current:%5.2fA Voltage:%5.2fV power:%5.2fW", i+1, curr, volt, watt);
                s += buffer;
                s += i < 2 ? "\n":"";
            }
            return s;
        };

        static INA3221* ina_0; 

    public:
        static void init(ina3221_addr_t addr) {
            ina_0 = new INA3221(addr);
            ina_0->begin(&Wire);
            ina_0->reset();
            ina_0->setShuntRes(100, 100, 100);
        }

        static void consumeElectricity() {
            logger.log(getText("").c_str());
        }

        static void electricityConsumption(AsyncWebServerRequest *request){
            String status = "Electricity Consumption is\n";
            status += getText(status);
            request->send(200, "text/plain", status);
        }

};

#endif