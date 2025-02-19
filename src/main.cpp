#include <WiFi.h>
#include <limits.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <esp_task_wdt.h>
#include <Wire.h>


#include <INA3221.h>

#include <L298N.h>
#include <median3.h>

#include "configuration.h"
#include "secret.h"

#include "WiFiContext.h"

#include "Logger.h"
#include "commonFunctions.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

WiFiContext wifiContext;
Logger logger;
AsyncWebServer server(80);

GMedian3<float> medianFilter;

uint16_t value = 0; // variable to store the sensor value
float level = 0;    // variable to store the water level

unsigned long duration;
float distanceCm;
float distanceInch;

 //Create an instance of the HardwareSerial class for Serial 2

INA3221 ina_0(INA3221_ADDR40_GND);
L298N m1(motor1In1, motor1In2, motor1En, PWM_CHB, false);
TimerHandle_t _consumeElectricity = NULL;


void consumeElectricity();
void electricityConsumption(AsyncWebServerRequest *request);

void current_measure_init() {
    ina_0.begin(&Wire);
    ina_0.reset();
    ina_0.setShuntRes(100, 100, 100);
}

void initPumnIn()
{
  pinMode(pumpInPin, OUTPUT);
  pinMode(pumpInLedOnPin, OUTPUT);
  pinMode(pumpInLedOffPin, OUTPUT);
  digitalWrite(pumpInPin, LOW);
  digitalWrite(pumpInLedOnPin, LOW);
  digitalWrite(pumpInLedOffPin, LOW);
}

void PumnInOn()
{
  digitalWrite(pumpInPin, HIGH);
  digitalWrite(pumpInLedOnPin, HIGH);
  digitalWrite(pumpInLedOffPin, LOW);
}

void PumnInOff()
{
  digitalWrite(pumpInPin, LOW);
  digitalWrite(pumpInLedOnPin, LOW);
  digitalWrite(pumpInLedOffPin, HIGH);
}

void PumnOutOn()
{
  digitalWrite(pumpOutPin, HIGH);
  digitalWrite(pumpOutLedOnPin, HIGH);
  digitalWrite(pumpOutLedOffPin, LOW);
}

void PumnOutOff()
{
  digitalWrite(pumpOutPin, LOW);
  digitalWrite(pumpOutLedOnPin, LOW);
  digitalWrite(pumpOutLedOffPin, HIGH);
}

void initPumnOut()
{
  pinMode(pumpOutPin, OUTPUT);
  pinMode(pumpOutLedOnPin, OUTPUT);
  pinMode(pumpOutLedOffPin, OUTPUT);
  digitalWrite(pumpOutPin, LOW);
  digitalWrite(pumpOutLedOnPin, LOW);
  digitalWrite(pumpOutLedOffPin, LOW);
}

void initWaterSensor()
{
  pinMode(waterSensorPowerPin, OUTPUT);   // configure D7 pin as an OUTPUT
  digitalWrite(waterSensorPowerPin, LOW); // turn the sensor OFF
}

uint16_t readWaterSensorValue()
{
  digitalWrite(waterSensorPowerPin, HIGH);  // turn the sensor ON
  delay(10);                                // wait 10 milliseconds
  value = analogRead(waterSensorSignalPin); // read the analog value from sensor
  digitalWrite(waterSensorPowerPin, LOW);   // turn the sensor OFF
  return value;
}

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

  _consumeElectricity = xTimerCreate(PSTR("con"), _consumeElectricityPeriod, pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(consumeElectricity));
  xTimerStart(_consumeElectricity, 0);

  initPumnIn();
  initPumnOut();
  initWaterSensor();

//  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output     
//  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  Serial2.begin(sonic_baund_rate, SERIAL_8N1, sonicRX, sonicTX);

  current_measure_init();

  m1.begin();

//  esp_task_wdt_init(WDT_TIMEOUT, true); // Initialize ESP32 Task WDT
//  esp_task_wdt_add(NULL);               // Subscribe to the Task WDT

}

void loop()
{

  value = 0;
/*
  Serial.println("start read");
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(20);
  // Sets the trigPin on HIGH state for 10 micro seconds/
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(160);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
    // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
    
  // Prints the distance in the Serial Monitor
  Serial.println(duration);
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
*/
  int av = Serial2.available();
  Serial.println(av);
  if (av >= 4) {
        uint8_t data[4];
        for (int i = 0; i < 4; i++) {
            data[i] = Serial2.read();
            Serial.print(data[i]);
        }
        Serial.println("");
        if (data[0] == 0xFF) {  // Check packet start byte
            int sum = (data[0] + data[1] + data[2]) & 0xFF;
            if (sum == data[3]) {  // Validate checksum
                int distance = (data[1] << 8) + data[2];  // Calculate distance
                if (distance > 30) {  // Ensure valid reading
                    Serial.print("Distance: ");
                    Serial.print(distance / 10.0);
                    Serial.println(" cm");
                } else {
                    Serial.println("Below the lower limit");
                }
            } else {
                Serial.println("Checksum error");
            }
        }
  } else  Serial.println("data not available");
  delay(100);


/*
  Serial.println("Avanti dritto a velocità variabile");
  m1.forward(); 
  m1.setSpeed(100); 
  delay(1000);

  for(int i=0; i<=100; i++) {
    m1.setSpeed(i); 
    delay(50);
  }
  Serial.println("Indietro dritto a velocità variabile");

  m1.reverse(); 
  for(int i=0; i<=100; i++) {
    m1.setSpeed(i); 
    delay(50);
  }
  
  Serial.println("Brake");
  m1.brake(); 
  delay(1000);
*/


/*

  if (abs(level - normalLevel) <= deviationWater)
  {
    logger.log(PSTR("Normal: Distance in mm: %f"), level);
    PumnInOn();
    PumnOutOn();
  }
  else if (level > (normalLevel + deviationWater))
  {
    logger.log(PSTR("In pumn off: Distance in mm: %f"), level);
    PumnInOff();
    PumnOutOn();
  }
  else if (level < (normalLevel - deviationWater))
  {
    logger.log(PSTR("Out pumn off: Distance in mm: %f"), level);
    PumnInOn();
    PumnOutOff();
  }
  else
  {
    logger.log(PSTR("Pump off %f"), level);
    PumnInOff();
    PumnOutOff();
  }
*/

//  esp_task_wdt_reset();
//  delay(1000);

}

void consumeElectricity()
{
    logger.log("A1: Current:%5.0f ma Voltage:%3.1fV", ina_0.getCurrent(INA3221_CH1) * 1000, ina_0.getVoltage(INA3221_CH1));
    logger.log("A2: Current:%5.0f ma Voltage:%3.1fV", ina_0.getCurrent(INA3221_CH2) * 1000, ina_0.getVoltage(INA3221_CH2));
    logger.log("A3: Current:%5.0f ma Voltage:%3.1fV", ina_0.getCurrent(INA3221_CH3) * 1000, ina_0.getVoltage(INA3221_CH3));
//    xTimerStart(_consumeElectricity, 0);
}

void electricityConsumption(AsyncWebServerRequest *request)
{
  String status = "Hi! I am ";
  status += nameboard;
  status += "\n";
  status += "Electricity Consumption is\n";
  for (uint8_t i = 0; i++; i < 3) {
    auto curr = ina_0.getCurrent((ina3221_ch_t)i );
    auto volt = ina_0.getVoltage((ina3221_ch_t)i);
    auto watt = curr * volt;
    char buffer[40];
    int len = sprintf(buffer, "A%d: Current:%5.0f ma Voltage:%3.1fV power:%3.1fwt\n", i+1, curr, volt, watt);
    status += buffer;
  }
  request->send(200, "text/plain", status);
}
