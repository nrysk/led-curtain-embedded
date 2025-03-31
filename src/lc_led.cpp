#include <LittleFS.h>
#include <M5AtomS3.h>
#include <FastLED.h>
#include <pngle.h>
#include "lc_fs.h"

#define NUM_LEDS 800
#define DATA_PIN 8
#define MAX_POWER_MILLIAMPS 500
#define LED_TYPE WS2812B
#define COLOR_ORDER RGB
#define LED_WIDTH 20
#define LED_HEIGHT 40

CRGB leds[NUM_LEDS];

/**
 * @brief LED の初期化を行う関数
 */
void setupLED()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
}

static void draw_callback(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, unsigned char *rgba)
{
    if (x >= LED_WIDTH || y >= LED_HEIGHT)
    {
        return;
    }
    leds[x * LED_HEIGHT + y] = CRGB(rgba[0], rgba[1], rgba[2]);
}

/**
 * @brief LED マトリクスに画像を表示する関数
 * @param id プリセット ID
 * @param frameIndex フレームのインデックス
 */
void lightUpImage(const int id, const int frameIndex)
{
    String path = getFilePath(String(id), String(frameIndex));
    if (!LittleFS.exists(path))
    {
        Serial.println("Image not found");
        return;
    }

    pngle_t *pngle = pngle_new();
    pngle_set_draw_callback(pngle, draw_callback);

    File file = LittleFS.open(path, "r");
    uint8_t buffer[1024];
    while (file.available())
    {
        size_t len = file.read(buffer, sizeof(buffer));
        if (len <= 0)
        {
            break;
        }
        pngle_feed(pngle, buffer, len);
    }

    pngle_destroy(pngle);
    file.close();

    FastLED.show();
}
