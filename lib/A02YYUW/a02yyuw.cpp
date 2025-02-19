#include "a02yyuw.h"
#include "Logger.h"

extern Logger logger;

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

TimerHandle_t a02yyuw::_getValueTimer = NULL;
SemaphoreHandle_t a02yyuw::shared_var_mutex = NULL;

void a02yyuw::init(u_int16_t rxPin, u_int16_t txPin, uint32_t bound_rate, boolean debug = false) {
    _rxPin = rxPin;
    _txPin = txPin;
    _bound_rate = bound_rate;
    _DEBUG = debug;
    _getValueTimer = xTimerCreate(PSTR("wifiTimer"), us_get_value_period, pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(getValue));
    shared_var_mutex = xSemaphoreCreateMutex();  // Create the mutex
   Serial2.begin(_bound_rate, SERIAL_8N1, _rxPin, _txPin);

}

void a02yyuw::getValue() {
  if (Serial2.available() >= 4) {
        uint8_t data[4];
        for (int i = 0; i < 4; i++) {
            data[i] = Serial2.read();
            if (_DEBUG) Serial.print(data[i]);
        }
        if (_DEBUG) Serial.println("");
        if (data[0] == 0xFF) {  // Check packet start byte
            int sum = (data[0] + data[1] + data[2]) & 0xFF;
            if (sum == data[3]) {  // Validate checksum
                int distance = (data[1] << 8) + data[2];  // Calculate distance
                if (distance > 30) {  // Ensure valid reading
                    if (_DEBUG) Serial.print("Distance: ");
                    if (_DEBUG) Serial.print(distance / 10.0);
                    if (_DEBUG) Serial.println(" cm");
                    a02yyuw::setDistance(distance);
                } else {
                    if (_DEBUG) Serial.println("Below the lower limit");
                    a02yyuw::setDistance(0);
                }
            } else {
                if (_DEBUG) Serial.println("Checksum error");
                a02yyuw::setDistance(0);
            }
        }
  } else { 
    if (_DEBUG) Serial.println("data not available");
  }
}

void a02yyuw::stopTimer() {
    if (xTimerIsTimerActive(_getValueTimer) == pdTRUE) {
        logger.log(PSTR("a02yyuw::stopTimer"));
        xTimerStart(_getValueTimer, 0);
    }
}

void a02yyuw::startTimer() {
    if (xTimerIsTimerActive(_getValueTimer) == pdFALSE) {
        logger.log(PSTR("a02yyuw::startTimer"));
        xTimerStart(_getValueTimer, 0);
    }
}