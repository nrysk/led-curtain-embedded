#include <SPIFFS.h>

bool successFS = false;

/**
 * @brief ファイルシステムの初期化を行う関数
 */
void setupFS()
{
    Serial.println("Setting up file system");

    if (!SPIFFS.begin(true))
    {
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }

    successFS = true;

    Serial.println("File system set up");
}
