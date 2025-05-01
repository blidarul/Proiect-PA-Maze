#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "maze.h"
#include "gameResources.h"

// Constants for rendering
#define MINIMAP_SCALE 5.0f
#define PLAYER_COLOR RED
#define MINIMAP_DIST_FROM_BORDER 20.0f

// Initialize the window and OpenGL context
void InitializeWindow(int width, int height, const char* title);

// Render the frame with 3D scene and UI
void RenderFrame(Camera camera, GameResources resources, Root root, int playerCellX, int playerCellY);

// Draw the minimap UI
void DrawMinimap(GameResources resources, int playerCellX, int playerCellY);

#endif // RENDERER_H