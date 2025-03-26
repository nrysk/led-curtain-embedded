#include <WiFi.h>
#include <M5AtomS3.h>
#include "lc_config.h"

/**
 * @brief WiFi の設定を行う関数
 */
void setupWiFi()
{
    Serial.printf("Connecting to %s", WIFI_SSID);

    // WiFi モードを STA に設定
    WiFi.mode(WIFI_STA);

    // WiFi に接続
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++)
    {
        delay(500);
        Serial.print(".");
    }

    // 接続に失敗した場合はエラーメッセージを表示
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Failed to connect to WiFi");
        return;
    }

    // 自動再接続を有効にする
    WiFi.setAutoReconnect(true);

    // 接続に成功した場合は IP アドレスを表示
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("WiFi set up");
}
