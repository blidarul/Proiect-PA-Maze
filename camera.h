#ifndef CAMERA_H
#define CAMERA_H


#include "raylib.h"
#include "gameResources.h"
#include "config.h"



// Initialize the camera with default settings
Camera InitializeCamera(void);

// Handle player movement based on input
void UpdatePlayerMovement(Camera* camera, GameResources resources);

void DrawStaminaBar();

#endif // CAMERA_H