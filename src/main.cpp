#include <WiFi.h>
#include <limits.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"


#include <L298N.h>
#include "a02yyuw.h"
#include "pump.h"

#include "configuration.h"
#include "secret.h"

#include "WiFiContext.h"

#include "Logger.h"
#include "commonFunctions.h"
#include <INA3221.h>
#include "electricityConsumption.h"


WiFiContext wifiContext;
Logger      logger;
AsyncWebServer server(80);
a02yyuw sonic_sensor;
Pump    pump;
ElectricityConsumption electricityConsumption;


void setup()
{
  Serial.begin(115200);
  while (!Serial){}
  Serial.println("__ OK __");
  logger.logInitialization();
//  wifiContext.initializate(_SSID, _PASS);

//  server.on("/log", HTTP_GET, Logger::publishLog);
//  server.on("/memory", HTTP_GET, getFreeHeapSize);
//  server.on("/electricity", HTTP_GET, electricityConsumption);

//  AsyncElegantOTA.begin(&server); // Start ElegantOTA
//  server.begin();

  Serial.println("HTTP server started");
  Serial.println("Setup Ended");
  Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");

  pump.init(pumpInIn1Pin, pumpInIn2Pin, pumpInEnPin, pumpInLedOnPin, pumpInLedOffPin
                    ,pumpOutIn1Pin, pumpOutIn2Pin, pumpOutEnPin, pumpOutLedOnPin, pumpOutLedOffPin, true);
  sonic_sensor.init(sonicRX, sonicTX, sonic_baund_rate, false);
  electricityConsumption.init(INA3221_ADDR40_GND);

  esp_task_wdt_init(WDT_TIMEOUT, true); // Initialize ESP32 Task WDT
  esp_task_wdt_add(NULL);               // Subscribe to the Task WDT

}

void loop() {
  pump.processing();
  electricityConsumption.consumeElectricity();
  esp_task_wdt_reset();
  delay(1000);

}

