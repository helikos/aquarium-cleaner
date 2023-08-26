

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "configuration.h"
#include "Logger.h"

extern Logger logger;



void getFreeHeapSize(AsyncWebServerRequest *request)
{
  String status = "Hi! I am alarm map.\n";
  status += "Free heap memory size is ";
  status += String(esp_get_free_heap_size());
  status += " bytes \n";
  request->send(200, "text/plain", status);
}
