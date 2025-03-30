#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <pngle.h>
#include <ArduinoJson.h>
#include "lc_config.h"
#include "lc_fs.h"

#define IMAGE_WIDTH 20
#define IMAGE_HEIGHT 20
#define MIN_INTERVAL 100
#define MAX_INTERVAL 10000

AsyncWebServer server(80);

// ファイル書き込み用の変数
File file;
String frames;

// typedef std::function<void(AsyncWebServerRequest *request)> ArRequestHandlerFunction;
// typedef std::function<void(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)>
// ArUploadHandlerFunction;
// typedef std::function<void(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)> ArBodyHandlerFunction;

void handlePostFrame(AsyncWebServerRequest *request)
{
    const String id = request->pathArg(0);
    if (request->hasParam("frames", true))
    {
        frames = request->getParam("frames", true)->value();
        Serial.printf("POST /presets/%s/frames, frames: %s\n", id, frames);
    }
    else
    {
        Serial.println("No frames parameter");
        request->send(400, "text/plain", "No frames parameter");
    }
}
void handlePostFrameUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (index == 0)
    {
        const String id = request->pathArg(0);

        // ID が有効な範囲かどうかを確認
        if (id.toInt() <= 0 || id.toInt() > MAX_PRESET_NUM)
        {
            Serial.println("Invalid content ID");
            request->send(400, "text/plain", "Invalid content ID");
            return;
        }
        // フレーム数が有効な範囲かどうかを確認
        if (frames.toInt() < 0 || frames.toInt() > MAX_FRAME_NUM)
        {
            Serial.println("Invalid frame number");
            request->send(400, "text/plain", "Invalid frame number");
            return;
        }

        // 画像が png であるか確認
        pngle_t *pngle = pngle_new();
        if (pngle_feed(pngle, data, len) < 0)
        {
            Serial.println("Invalid PNG file");
            request->send(400, "text/plain", "Invalid PNG file");
            pngle_destroy(pngle);
            return;
        }
        // 画像のサイズを確認
        pngle_ihdr_t *ihdr = pngle_get_ihdr(pngle);
        if (!ihdr || ihdr->width > IMAGE_WIDTH || ihdr->height > IMAGE_HEIGHT)
        {
            Serial.printf("Image size exceeds limit: %dx%d\n", ihdr->width, ihdr->height);
            request->send(400, "text/plain", "Image size exceeds limit");
            pngle_destroy(pngle);
            return;
        }
        pngle_destroy(pngle);

        // 画像の保存先を指定
        Serial.printf("Receiving file %s\n", filename.c_str());
        file = openImageFile(id, frames);
        if (!file)
        {
            Serial.println("Failed to open file for writing");
            request->send(500, "text/plain", "Failed to open file for writing");
            return;
        }
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
}

void handlePostTotalFrames(AsyncWebServerRequest *request) {}
void handlePostTotalFramesBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    // ID が有効な範囲かどうかを確認
    const String id = request->pathArg(0);
    if (id.toInt() <= 0 || id.toInt() > MAX_PRESET_NUM)
    {
        Serial.println("Invalid content ID");
        request->send(400, "text/plain", "Invalid content ID");
        return;
    }

    // JSON データのパース
    String jsonData = String((char *)data);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonData);
    if (error)
    {
        Serial.println("Failed to parse JSON");
        request->send(400, "text/plain", "Failed to parse JSON");
        return;
    }

    // 総フレーム数の取得
    int totalFrames = doc["totalFrames"];
    if (totalFrames < 0 || totalFrames > MAX_FRAME_NUM)
    {
        Serial.println("Invalid frame number");
        request->send(400, "text/plain", "Invalid frame number");
        return;
    }

    // フレーム間インターバルの取得
    int interval = doc["interval"];
    if (interval < MIN_INTERVAL || interval > MAX_INTERVAL)
    {
        Serial.println("Invalid interval");
        request->send(400, "text/plain", "Invalid interval");
        return;
    }

    Serial.printf("POST /presets/%s, totalFrames: %d, interval: %d\n", id.c_str(), totalFrames, interval);
    setTotalFrames(id, totalFrames);
    setInterval(id, interval);
    request->send(200, "text/plain", "OK");
}

/**
 * @brief サーバーの設定を行う関数
 */
void setupServer()
{
    Serial.println("Setting up server");

    // CORS の設定
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    // POST /presets/:id/frames => 応する画像を保存・更新
    server.on(
        "^\\/presets\\/([0-9]+)\\/frames$", HTTP_POST,
        handlePostFrame,
        handlePostFrameUpload);

    // POST /presets/:id => 対応するプリセットの総フレーム数を設定
    server.on(
        "^\\/presets\\/([0-9]+)$", HTTP_POST,
        handlePostTotalFrames,
        nullptr,
        handlePostTotalFramesBody);

    // サーバの開始
    server.begin();

    Serial.println("Server started");
}
