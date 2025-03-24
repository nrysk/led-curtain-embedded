#include <Arduino.h>
#include <M5AtomS3.h>
#include <WiFi.h>

#include "lc_server.h"
#include "lc_wifi.h"
#include "lc_ntp.h"

enum class State
{
  Normal,
  QRCode,
};

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
  static State state = State::Normal;
  static State prevState = State::QRCode;

  AtomS3.update();

  if (AtomS3.BtnA.pressedFor(1000))
  {
    // ボタン A を長押し：QR コード表示モードに切り替え
    Serial.println("Switching to QR code display mode");
    state = State::QRCode;
  }
  else if (AtomS3.BtnA.wasPressed())
  {
    // ボタン A を押下：通常表示モードに切り替え
    Serial.println("Switching to normal display mode");
    state = State::Normal;
  }

  if (state != prevState)
  {
    // 状態が変化した場合はディスプレイを更新
    switch (state)
    {
    case State::Normal:
      // 通常表示モード
      AtomS3.Display.startWrite();
      AtomS3.Display.clear();
      AtomS3.Display.setCursor(0, 0);
      AtomS3.Display.println(WiFi.localIP());
      AtomS3.Display.endWrite();
      break;

    case State::QRCode:
      // QR コード表示モード
      AtomS3.Display.startWrite();
      AtomS3.Display.clear();
      AtomS3.Display.setCursor(0, 0);
      AtomS3.Display.setTextSize(1.5);
      AtomS3.Display.qrcode("http://" + WiFi.localIP().toString());
      AtomS3.Display.endWrite();
      break;
    }
  }

  prevState = state;
  delay(100);
}
