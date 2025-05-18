#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "maze.h"
#include "gameResources.h"
#include "config.h"

// Initialize the window and OpenGL context
void InitializeWindow(int width, int height, const char* title);

// Render the frame with 3D scene and UI
void RenderFrame(Camera camera, GameResources resources, Root root, int playerCellX, int playerCellY);

// Draw the minimap UI
void DrawMinimap(GameResources resources, int playerCellX, int playerCellY);

// Draw stamina bar
void DrawStaminaBar(float currentStamina, float maxStamina);

#endif // RENDERER_H