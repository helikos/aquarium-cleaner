#include <WiFi.h>
#include <limits.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "Adafruit_VL53L0X.h"
#include <esp_task_wdt.h>

#include <runningAverage.h>
#include <median3.h>


#include "configuration.h"
#include "secret.h"

#include "WiFiContext.h"

#include "Logger.h"
#include "commonFunctions.h"

WiFiContext wifiContext;
Logger logger;
AsyncWebServer server(80);

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

GMedian3<float> medianFilter;
GFilterRA runningAverage;



void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  Serial.println("__ OK __");
  logger.logInitialization();
  wifiContext.initializate(_SSID, _PASS);

  server.on("/log", HTTP_GET, Logger::publishLog);
  server.on("/memory", HTTP_GET, getFreeHeapSize);
  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  Serial.println("Setup Ended");
  Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");

  Serial.println("Adafruit VL53L0X test.");
  if (!lox.begin())
  {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  // power
  Serial.println(F("VL53L0X API Continuous Ranging example\n\n"));
  runningAverage.setCoef(0.5);
  runningAverage.setStep(100);

  // start continuous ranging
  lox.startRangeContinuous();

  pinMode(pumpPinIn, OUTPUT); // Set GPIO22 as digital output pin
  pinMode(pumpPinOut, OUTPUT); // Set GPIO22 as digital output pin

  esp_task_wdt_init(WDT_TIMEOUT, true); // Initialize ESP32 Task WDT
  esp_task_wdt_add(NULL);               // Subscribe to the Task WDT

//  digitalWrite(pumpPinOut, HIGH); 
//  digitalWrite(pumpPinIn, HIGH); 

}

void loop()
{
  
  if (lox.isRangeComplete())
  {
//    Serial.print("Distance in mm: ");
    uint16_t range = lox.readRange();
    float avgRange =  medianFilter.filtered((float)range );

    if ( abs(avgRange - rangeWater) <= deviationWater ) {
      logger.log(PSTR("Normal: Distance in mm: %f"),  avgRange);
      digitalWrite(pumpPinOut, HIGH); 
      digitalWrite(pumpPinIn, HIGH); 
    }

    if ( (avgRange - rangeWater) > deviationWater4Motor + deviationWater) {
      logger.log(PSTR("Out pumn off: Distance in mm: %f"), avgRange);
      digitalWrite(pumpPinOut, LOW); 
      digitalWrite(pumpPinIn, HIGH); 
    } 
    if ( (rangeWater - avgRange) > deviationWater4Motor + deviationWater) {
      logger.log(PSTR("In pumn off: Distance in mm: %f"), avgRange);
      digitalWrite(pumpPinOut, HIGH); 
      digitalWrite(pumpPinIn, LOW); 
    } 
    esp_task_wdt_reset();
    delay(3000);
  }
/*  
  else {
    digitalWrite(pumpPinOut, LOW); 
    digitalWrite(pumpPinIn, LOW); 
  }
*/  

}
