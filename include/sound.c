#include "sound.h"
#include <stdio.h>
#include "raylib.h"
#include "config.h"

static Sound steps[STEP_SOUND_COUNT];
static float stepTimer = 0.0f;
static int currentStep = 0;

static Music bgm;
static float currentBgmVolume = 0.5f;

void InitBGM(const char *musicPath)
{
    bgm = LoadMusicStream(musicPath);
    PlayMusicStream(bgm);
    SetMusicVolume(bgm, currentBgmVolume);
}

void UpdateBGM(void)
{
    UpdateMusicStream(bgm);
}

void UnloadBGM(void)
{
    StopMusicStream(bgm);
    UnloadMusicStream(bgm);
}

void SetCurrentMusicVolume(float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    currentBgmVolume = volume;
    SetMusicVolume(bgm, currentBgmVolume);
}

float GetCurrentMusicVolume(void)
{
    return currentBgmVolume;
}

void InitStepSounds(const char *directory)
{
    char path[256];
    for (int i = 0; i < STEP_SOUND_COUNT; i++)
    {
        snprintf(path, sizeof(path), "%s/Steps_%d.wav", directory, i + 1);
        steps[i] = LoadSound(path);
    }
}

void UpdateStepSounds()
{
    float delta = GetFrameTime();
    stepTimer -= delta;

    bool isMoving = IsKeyDown(KEY_W) || IsKeyDown(KEY_A) ||
                    IsKeyDown(KEY_S) || IsKeyDown(KEY_D);

    if (isMoving && stepTimer <= 0.0f)
    {
        PlaySound(steps[currentStep]);
        currentStep = (currentStep + 1) % STEP_SOUND_COUNT;
        stepTimer = STEP_COOLDOWN;
    }
}

void UnloadStepSounds()
{
    for (int i = 0; i < STEP_SOUND_COUNT; i++)
        UnloadSound(steps[i]);
}