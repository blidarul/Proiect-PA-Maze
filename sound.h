#ifndef SOUND_H
#define SOUND_H
#define STEP_SOUND_COUNT 4
#define STEP_COOLDOWN 0.3f

void InitStepSounds(const char *directory);
void UpdateStepSounds();
void UnloadStepSounds();
void InitBGM(const char *musicPath);
void UpdateBGM();
void UnloadBGM();

#endif