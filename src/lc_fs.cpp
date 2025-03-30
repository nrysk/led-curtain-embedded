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

/**
 * @brief ファイルパスを取得する関数
 * @param id プリセット ID
 * @param frameIndex フレームのインデックス
 * @return ファイルパス
 */
String getFilePath(const String id, const String frameIndex)
{
    return "/presets/" + id + "/image" + String(frameIndex) + ".png";
}

/**
 * @brief 画像ファイルを開く関数
 * @param id プリセット ID
 * @param frameIndex フレームのインデックス
 * @return ファイルオブジェクト
 */
File openImageFile(const String id, const String frameIndex)
{
    return LittleFS.open(getFilePath(id, frameIndex), FILE_WRITE, true);
}

/**
 * @brief 対応するプリセットの総フレーム数を設定する関数
 * @param id プリセット ID
 * @param totalFrames 総フレーム数
 */
void setTotalFrames(const String id, const int totalFrames)
{
    String path = "/presets/" + id + "/totalFrames";
    File file = LittleFS.open(path, FILE_WRITE, true);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.print(totalFrames);
    file.close();
}

/**
 * @brief 対応するプリセットの総フレーム数を取得する関数
 * @param id プリセット ID
 * @return 総フレーム数
 */
int getTotalFrames(const String id)
{
    String path = "/presets/" + id + "/totalFrames";
    if (!LittleFS.exists(path))
    {
        Serial.println("Total frames file not found");
        return -1;
    }
    File file = LittleFS.open(path, FILE_READ);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return -1;
    }
    String totalFrames = file.readStringUntil('\n');
    file.close();
    return totalFrames.toInt();
}

/**
 * @brief 対応するプリセットのフレーム間インターバルを設定する関数
 * @param id プリセット ID
 * @param interval インターバル
 */
void setInterval(const String id, const int interval)
{
    String path = "/presets/" + id + "/interval";
    File file = LittleFS.open(path, FILE_WRITE, true);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.print(interval);
    file.close();
}

/**
 * @brief 対応するプリセットのフレーム間インターバルを取得する関数
 * @param id プリセット ID
 * @return インターバル
 */
int getInterval(const String id)
{
    String path = "/presets/" + id + "/interval";
    if (!LittleFS.exists(path))
    {
        Serial.println("Interval file not found");
        return -1;
    }
    File file = LittleFS.open(path, FILE_READ);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return -1;
    }
    String interval = file.readStringUntil('\n');
    file.close();
    return interval.toInt();
}