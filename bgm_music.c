#include "bgm_music.h"

static Music bgm;

void InitBGM(const char *musicPath)
{
    bgm = LoadMusicStream(musicPath);
    PlayMusicStream(bgm);
}

void UpdateBGM()
{
    UpdateMusicStream(bgm);
}

void UnloadBGM()
{
    StopMusicStream(bgm);
    UnloadMusicStream(bgm);
}