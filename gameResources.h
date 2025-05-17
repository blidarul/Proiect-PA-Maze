#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H
#include "raylib.h"
#include "maze.h"
#include "questions.h"

// Structure to hold game resources
typedef struct
{
    Model model;
    Mesh mesh;
    Texture2D texture;
    Texture2D cubicmap;
    Texture2D minimapTexture;
    Color *mapPixels;
    Image minimap;
    Image cubicimage;
    Question questions[60];
} GameResources;

// Load all game resources
GameResources LoadGameResources(Maze *maze, int height, int width);

// Free resources to prevent memory leaks
void UnloadGameResources(GameResources* resources);

// Clean up all resources including maze data
void CleanupResources(GameResources* resources, Maze *maze, int height);

void UpdateMinimapTexture(GameResources* resources);

#endif // GAME_RESOURCES_H