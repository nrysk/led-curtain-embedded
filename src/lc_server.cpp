#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "lc_config.h"

#define PNG_SIGNATURE 0x474e5089

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

    // POST /contents/:id へのリクエストを受けた場合は、ファイルを受信
    server.on(
        "^\\/contents\\/([0-9]+)$", HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            const String id = request->pathArg(0);
            Serial.printf("POST /contents/%d\n", id.toInt());
        },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            const String id = request->pathArg(0);
            // ID が有効な範囲かどうかを確認
            if (id.toInt() < 0 || id.toInt() > MAX_CONTENT_NUM)
            {
                Serial.println("Invalid content ID");
                request->send(400, "text/plain", "Invalid content ID");
                return;
            }
            if (index == 0)
            {
                Serial.printf("Receiving file %s\n", filename.c_str());
                // ファイルは png 画像に限定
                if (!filename.endsWith(".png") || ((uint32_t *)data)[0] != PNG_SIGNATURE)
                {
                    Serial.println("Only PNG files are accepted");
                    request->send(400, "text/plain", "Only PNG files are accepted");
                    return;
                }
                file = LittleFS.open("/contents/" + id + "/image.png", FILE_WRITE, true);
            }

            if (file)
            {
                file.write(data, len);
            }

            if (final)
            {
                Serial.printf("Received file %s\n", filename.c_str());
                file.close();
                request->send(200, "text/plain", "Completed");
            }
        });

    // サーバの開始
    server.begin();

    Serial.println("Server started");
}
