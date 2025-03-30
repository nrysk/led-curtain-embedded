#ifndef LC_FS_H
#define LC_FS_H

extern bool successFS;

void setupFS();
String getFilePath(const String id, const String frames);
File openImageFile(const String id, const String frames);
void setTotalFrames(const String id, const int totalFrames);
int getTotalFrames(const String id);
void setInterval(const String id, const int interval);
int getInterval(const String id);

#endif