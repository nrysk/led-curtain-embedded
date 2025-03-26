#include <M5AtomS3.h>
#include <WiFi.h>
#include "lc_fs.h"

void displaySystemInfo(bool init)
{

    AtomS3.Display.startWrite();
    if (init)
    {
        AtomS3.Display.clear(BLACK);
    }

    /**
     * 現在時刻の表示
     */
    static int prevSec = -1;
    if (init)
    {
        AtomS3.Display.fillRect(0, 0, AtomS3.Display.width(), 32, BLUE);
    }
    struct tm timeinfo;
    if (getLocalTime(&timeinfo) && timeinfo.tm_sec != prevSec || init)
    {
        AtomS3.Display.setTextSize(2);
        AtomS3.Display.setTextColor(WHITE, BLUE);
        AtomS3.Display.setCursor(0, 0);
        AtomS3.Display.println(&timeinfo, "%Y-%m-%d");
        AtomS3.Display.println(&timeinfo, "%H:%M:%S");
    }
    prevSec = timeinfo.tm_sec;

    /**
     * WiFi の接続状況の表示
     */
    static int prevWiFiStatus = -1;
    int wifiStatus = WiFi.status();
    if (init)
    {
        prevWiFiStatus = -1;
    }
    // WiFi の接続状況が変化した場合のみ表示を更新
    if (wifiStatus != prevWiFiStatus || init)
    {
        AtomS3.Display.setCursor(0, 32 + 8);
        AtomS3.Display.setTextSize(2);
        if (wifiStatus == WL_CONNECTED)
        {
            AtomS3.Display.setTextColor(GREEN);
            AtomS3.Display.print("+");
        }
        else
        {
            AtomS3.Display.setTextColor(RED);
            AtomS3.Display.println("x");
        }
        AtomS3.Display.setTextColor(WHITE);
        AtomS3.Display.println("WiFi");
        prevWiFiStatus = wifiStatus;
    }

    /**
     * ファイルシステムの初期化状況の表示
     */
    // ファイルシステムの成功は変化しないため、初回のみ表示
    if (init)
    {
        AtomS3.Display.setTextSize(2);
        if (successFS)
        {
            AtomS3.Display.setTextColor(GREEN);
            AtomS3.Display.print("+");
        }
        else
        {
            AtomS3.Display.setTextColor(RED);
            AtomS3.Display.print("x");
        }
        AtomS3.Display.setTextColor(WHITE);
        AtomS3.Display.println("FS");
    }

    /**
     * IP アドレスの表示
     */
    static IPAddress prevIP = -1;
    IPAddress ip = WiFi.localIP();
    // 画面の下部に表示
    if (init)
    {
        prevIP = -1;
        AtomS3.Display.fillRect(0, AtomS3.Display.height() - 8, AtomS3.Display.width(), 8, BLUE);
    }
    // IP アドレスが変化した場合のみ表示を更新
    if (ip != prevIP || init)
    {
        AtomS3.Display.setCursor(0, AtomS3.Display.height() - 8);
        AtomS3.Display.setTextSize(1);
        AtomS3.Display.setTextColor(WHITE, BLUE);
        AtomS3.Display.print("IP: ");
        AtomS3.Display.println(ip.toString());
        prevIP = ip;
    }

    AtomS3.Display.endWrite();
}

void displayQRCode(bool init)
{
    static IPAddress prevIP = -1;
    IPAddress ip = WiFi.localIP();
    if (ip == prevIP && !init)
    {
        return;
    }
    prevIP = ip;

    AtomS3.Display.startWrite();
    if (init)
    {
        AtomS3.Display.clear();
    }
    AtomS3.Display.qrcode("http://" + ip.toString());
    AtomS3.Display.endWrite();
}