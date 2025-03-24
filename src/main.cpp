#include <Arduino.h>
#include <M5AtomS3.h>

#include "lc_server.h"
#include "lc_wifi.h"
#include "lc_ntp.h"

void setup()
{
  AtomS3.begin();
  Serial.begin(115200);
  while (!Serial)
    ;

  setupWiFi();
  setupNTP();
  setupServer();
}

void loop()
{
}
