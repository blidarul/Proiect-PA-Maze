#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H
#define MAX_QUESTION_SIZE 200
#include "raylib.h"
#include "maze.h"

// Structure to hold game resources
typedef struct {
    char questionText[MAX_QUESTION_SIZE];
    char answers[3];
    char answersText[4][MAX_QUESTION_SIZE];
} Question;
typedef struct
{
    Model model;
    Mesh mesh;
    Texture2D texture;
    Texture2D cubicmap;
    Texture2D minimapTexture;
    Color *mapPixels;
    Vector3 mapPosition;
    Image minimap;
    Image cubicimage;
    int cellsVisited;
    Question questions[60];
} GameResources;

// Load all game resources
GameResources LoadGameResources(Cell** path, int height, int width);

// Free resources to prevent memory leaks
void UnloadGameResources(GameResources* resources);

// Clean up all resources including maze data
void CleanupResources(GameResources* resources, Cell** path, int height);

void UpdateMinimapTexture(GameResources* resources);

#endif // GAME_RESOURCES_H