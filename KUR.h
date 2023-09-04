#ifndef KUR_h
#define KUR_h

#include "Arduino.h"
#include <Ticker.h>
#include <SPIFFS.h>
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>


#include <Setting.h>


class KUR
{
public:
  KUR();  // Thêm hàm constructor
  ~KUR(); // Thêm hàm destructor

  void START_INIT(int EN_SERIAL = 0);
  void START_OTA();

  void RUN();

private:
};

#endif