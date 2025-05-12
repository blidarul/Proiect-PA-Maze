#ifndef BGM_MUSIC_H
#define BGM_MUSIC_H

#include "raylib.h"

void InitBGM(const char *musicPath);  // Pass "resources/music.wav"
void UpdateBGM();
void UnloadBGM();

#endif