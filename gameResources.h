#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H

#include "raylib.h"
#include "maze.h"
#include "types.h"

// Structure to hold game resources
struct GameResources {
    Model model;
    Mesh mesh;
    Texture2D texture;
    Texture2D cubicmap;
    Texture2D minimapTexture;
    Color *mapPixels;
    Image minimap;
    Image cubicimage;
    Question questions[60];
};

// Function declarations
GameResources LoadGameResources(Maze *maze, int height, int width);
void UnloadGameResources(GameResources* resources);
void CleanupResources(GameResources* resources, Maze *maze, int height);
void UpdateMinimapTexture(GameResources* resources);

#endif // GAME_RESOURCES_H