#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "maze.h"
#include "gameResources.h"
#include "config.h"

// Initialize the camera with default settings
Camera InitializeCamera(void);

// Handle player movement based on input
void UpdatePlayerMovement(Camera* camera, GameResources resources);

void DrawStaminaBar();

Camera InitializeCamera(void); // Ensure app_settings is initialized before this is called if it relies on it
void UpdatePlayerMovement(Camera *camera, GameResources resources);

#endif // CAMERA_H