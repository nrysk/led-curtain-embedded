#include <Arduino.h>
#include <M5AtomS3.h>
#include <FastLED.h>
#include <LittleFS.h>

#include "lc_config.h"
#include "lc_display.h"
#include "lc_server.h"
#include "lc_wifi.h"
#include "lc_ntp.h"
#include "lc_fs.h"
#include "lc_led.h"

enum class State
{
  Dummy,
  SystemInfo,
  QRCode,
  Image,
};

void setup()
{
  AtomS3.begin();
  Serial.begin(115200);
  delay(200);

  setupWiFi();
  setupNTP();
  setupFS();
  setupServer();
  setupLED();
}

void loop()
{
  static unsigned long previousMillis = 0;

  static State prevState = State::Dummy;
  static State state = State::SystemInfo;
  static int presetId = 0;
  static int frameIndex = 0;

  // デバイス状態の更新
  AtomS3.update();

  // ボタン A を長押し：QR コード表示モードに切り替え
  // ボタン A をダブルクリック：通常表示モードに切り替え
  // ボタン A をクリック：画像表示モードに切り替え
  if (AtomS3.BtnA.pressedFor(800))
  {
    state = State::QRCode;
  }
  else if (AtomS3.BtnA.wasDoubleClicked())
  {
    prevState = State::Dummy;
    state = State::SystemInfo;
  }
  else if (AtomS3.BtnA.wasPressed())
  {
    if (prevState == State::Image)
    {
      presetId = presetId % MAX_PRESET_NUM + 1;
    }
    prevState = State::Dummy;
    state = State::Image;
  }

  switch (state)
  {
  case State::SystemInfo:
    // 通常表示モード
    displaySystemInfo(prevState != state);
    break;

  case State::QRCode:
    // QR コード表示モード
    displayQRCode(prevState != state);
    break;

  case State::Image:
    // 一定時間ごとにフレームを進める
    const int totalFrames = getTotalFrames(String(presetId));
    const int interval = getInterval(String(presetId));
    unsigned long currentMillis = millis();
    if (totalFrames > 1 && interval >= 100 && currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      frameIndex = (frameIndex + 1) % totalFrames;
      prevState = State::Dummy;
    }

    // 画像表示モード
    displayImage(prevState != state, presetId, frameIndex);
    lightUpImage(presetId, frameIndex);
    break;
  }

  prevState = state;
  delay(100);
}
