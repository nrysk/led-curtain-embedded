#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "lc_config.h"

AsyncWebServer server(80);

// ファイル書き込み用の変数
File file;

/**
 * @brief サーバーの設定を行う関数
 */
void setupServer()
{
    Serial.println("Setting up server");

    // CORS の設定
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    // GET / へのリクエストを受けた場合は、フロントエンドの URL にリダイレクト
    server.on(
        "/",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            Serial.println("GET /");
        });

    server.on(
        "/content", HTTP_POST,
        [](AsyncWebServerRequest *request)
        { Serial.println("POST /content"); },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            if (index == 0)
            {
                Serial.printf("Uploading %s\n", filename.c_str());
                file = LittleFS.open("/content.png", FILE_WRITE);
            }

            if (file)
            {
                file.write(data, len);
            }

            if (final)
            {
                Serial.printf("Uploaded %s\n", filename.c_str());
                file.close();
                request->send(200, "text/plain", "Completed");
            }
        });

    // サーバの開始
    server.begin();

    Serial.println("Server started");
}
