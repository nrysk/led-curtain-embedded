#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "lc_config.h"

AsyncWebServer server(80);

/**
 * @brief サーバーの設定を行う関数
 *
 */
void setupServer()
{
    Serial.println("Setting up server");

    server.on(
        "/",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            Serial.println("GET /");
            request->redirect(FRONTEND_URL "?esp-ip=" + WiFi.localIP().toString());
        });

    server.begin();

    Serial.println("Server started");
}
