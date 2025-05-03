#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H

#include "raylib.h"
#include "maze.h"

// Structure to hold game resources
typedef struct
{
    Model model;
    Mesh mesh;
    Texture2D texture;
    Texture2D cubicmap;
    Texture2D minimapTexture;
    Color *mapPixels;
    Vector3 mapPosition;
} GameResources;

// Load all game resources
GameResources LoadGameResources(Cell** path, int height, int width);

// Free resources to prevent memory leaks
void UnloadGameResources(GameResources* resources);

// Clean up all resources including maze data
void CleanupResources(GameResources* resources, Cell** path, int height);

#endif // GAME_RESOURCES_H