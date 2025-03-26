#include <LittleFS.h>

bool successFS = false;

/**
 * @brief ファイルシステムの初期化を行う関数
 */
void setupFS()
{
    Serial.println("Setting up file system");

    if (!LittleFS.begin(true))
    {
        Serial.println("An error occurred while mounting FS");
        return;
    }

    successFS = true;

    Serial.println("File system set up");
}
