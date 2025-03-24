
#include <Arduino.h>
#include <time.h>
#include "lc_config.h"

const char *ntpServer1 = "ntp.nict.jp";
const char *ntpServer2 = "ntp.jst.mfeed.ad.jp";
const char *ntpServer3 = "time.google.com";
const long gmtOffset_sec = 9 * 3600;
const int daylightOffset_sec = 0;

/**
 * @brief NTP サーバーの設定を行う関数
 */
void setupNTP()
{
    Serial.println("Setting up NTP");

    // NTP サーバーに接続
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);

    // 現在時刻を取得
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }

    // 現在時刻を表示
    Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");

    Serial.println("NTP set up");
}