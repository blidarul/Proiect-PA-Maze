#ifndef SOUND_H
#define SOUND_H

#include "config.h"

void InitStepSounds(const char *directory);
void UpdateStepSounds();
void UnloadStepSounds();
void InitBGM(const char *musicPath);
void UpdateBGM();
void UnloadBGM();

#endif