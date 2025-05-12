#include "step_sounds.h"

#define STEP_SOUND_COUNT 4
#define STEP_COOLDOWN 0.3f

static Sound steps[STEP_SOUND_COUNT];
static float stepTimer = 0.0f;
static int currentStep = 0;

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