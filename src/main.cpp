#include <WiFi.h>
#include <limits.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <esp_task_wdt.h>

#include <median3.h>


#include "configuration.h"
#include "secret.h"

#include "WiFiContext.h"

#include "Logger.h"
#include "commonFunctions.h"

WiFiContext wifiContext;
Logger logger;
AsyncWebServer server(80);


GMedian3<float> medianFilter;

uint16_t value = 0; // variable to store the sensor value
float level = 0; // variable to store the water level

void initPumnIn() {
  pinMode(pumpInPin, OUTPUT); 
  pinMode(pumpInLedOnPin, OUTPUT); 
  pinMode(pumpInLedOffPin, OUTPUT); 
  digitalWrite(pumpInPin, LOW); 
  digitalWrite(pumpInLedOnPin, LOW); 
  digitalWrite(pumpInLedOffPin, LOW); 
}

void PumnInOn() {
  digitalWrite(pumpInPin, HIGH); 
  digitalWrite(pumpInLedOnPin, HIGH); 
  digitalWrite(pumpInLedOffPin, LOW); 
}

void PumnInOff() {
  digitalWrite(pumpInPin, LOW); 
  digitalWrite(pumpInLedOnPin, LOW); 
  digitalWrite(pumpInLedOffPin, HIGH); 
}


void PumnOutOn() {
  digitalWrite(pumpOutPin, HIGH); 
  digitalWrite(pumpOutLedOnPin, HIGH); 
  digitalWrite(pumpOutLedOffPin, LOW); 
}

void PumnOutOff() {
  digitalWrite(pumpOutPin, LOW); 
  digitalWrite(pumpOutLedOnPin, LOW); 
  digitalWrite(pumpOutLedOffPin, HIGH); 
}


void initPumnOut() {
  pinMode(pumpOutPin, OUTPUT); 
  pinMode(pumpOutLedOnPin, OUTPUT); 
  pinMode(pumpOutLedOffPin, OUTPUT); 
  digitalWrite(pumpOutPin, LOW); 
  digitalWrite(pumpOutLedOnPin, LOW); 
  digitalWrite(pumpOutLedOffPin, LOW); 
}

void initWaterSensor() {
  pinMode(waterSensorPowerPin, OUTPUT);   // configure D7 pin as an OUTPUT
  digitalWrite(waterSensorPowerPin, LOW); // turn the sensor OFF
}


uint16_t readWaterSensorValue() {
  digitalWrite(waterSensorPowerPin, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(waterSensorSignalPin); // read the analog value from sensor
  digitalWrite(waterSensorPowerPin, LOW);   // turn the sensor OFF
  return value;
}

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

  initPumnIn();
  initPumnOut();
  initWaterSensor();

  esp_task_wdt_init(WDT_TIMEOUT, true); // Initialize ESP32 Task WDT
  esp_task_wdt_add(NULL);               // Subscribe to the Task WDT

}

void loop()
{

  value = readWaterSensorValue();
  level = medianFilter.filtered(value);

  Serial.print("Water level: ");
  Serial.println(level);

  if ( abs(level - normalLevel) <= deviationWater ) {
    logger.log(PSTR("Normal: Distance in mm: %f"),  level);
    PumnInOn();
    PumnOutOn();
  } else if ( (level - normalLevel) > deviationWater4Motor) {
    logger.log(PSTR("In pumn off: Distance in mm: %f"), level);
    PumnInOff();
    PumnOutOn();
  } else if ( (normalLevel - level) > deviationWater4Motor) {
    logger.log(PSTR("Out pumn off: Distance in mm: %f"), level);
    PumnInOn();
    PumnOutOff();
  } else {
    logger.log(PSTR("Pump off %f"), level);
    PumnInOff();
    PumnOutOff();
  }

  esp_task_wdt_reset();
  delay(3000);

}
