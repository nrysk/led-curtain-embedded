#include <Arduino.h>
#include <M5AtomS3.h>

#include "lc_config.h"
#include "lc_display.h"
#include "lc_server.h"
#include "lc_wifi.h"
#include "lc_ntp.h"
#include "lc_fs.h"

enum class State
{
  Dummy,
  Normal,
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
}

void loop()
{
  static State prevState = State::Dummy;
  static State state = State::Normal;
  static int imageId = 0;

  // デバイス状態の更新
  AtomS3.update();

  if (AtomS3.BtnA.pressedFor(800))
  {
    // ボタン A を長押し：QR コード表示モードに切り替え
    Serial.println("Switching to QR code display mode");
    state = State::QRCode;
  }
  else if (AtomS3.BtnA.wasDoubleClicked())
  {
    // ボタン A をダブルクリック：通常表示モードに切り替え
    Serial.println("Switching to normal display mode");
    // 画面更新を強制するために前回の状態を Dummy に設定
    prevState = State::Dummy;
    state = State::Normal;
  }
  else if (AtomS3.BtnA.wasPressed())
  {
    // ボタン A を押下：画像表示モードに切り替え
    Serial.println("Switching to image display mode");
    // 既に画像表示モードの場合は、次の画像を表示
    if (prevState == State::Image)
    {
      imageId = (imageId + 1) % MAX_CONTENT_NUM;
    }
    // 画像表示モードに切り替え
    prevState = State::Dummy;
    state = State::Image;
  }

  switch (state)
  {
  case State::Normal:
    // 通常表示モード
    displaySystemInfo(prevState != state);
    break;

  case State::QRCode:
    // QR コード表示モード
    displayQRCode(prevState != state);
    break;

  case State::Image:
    // 画像表示モード
    displayImage(prevState != state, imageId);
    break;
  }

  prevState = state;
  delay(100);
}
