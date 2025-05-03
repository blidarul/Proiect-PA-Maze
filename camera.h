#ifndef CAMERA_H
#define CAMERA_H


#include "raylib.h"
#include "gameResources.h"
#include "config.h"

// Initialize the camera with default settings
Camera InitializeCamera(void);

// Handle player movement based on input
void UpdatePlayerMovement(Camera* camera, Vector3* oldCamPos, Vector3* oldCamTarget);

// Handle player collisions with walls
void HandleCollisions(Camera* camera, Vector3 oldCamPos, Vector3 oldCamTarget, 
                     GameResources resources, int playerCellX, int playerCellY);

#endif // CAMERA_H