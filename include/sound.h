#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"

void InitBGM(const char *musicPath);
void UpdateBGM(void);
void UnloadBGM(void);
void SetCurrentMusicVolume(float volume);
float GetCurrentMusicVolume(void);

void InitStepSounds(const char *directory);
void UpdateStepSounds(void);
void UnloadStepSounds(void);

#endif