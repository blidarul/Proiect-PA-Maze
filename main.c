#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "maze.h"
#include "camera.h"
#include "renderer.h"
#include "gameResources.h"

// Game constants
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define MAZE_SIZE 20


Cell** InitializeMazeData(int height, int width, Root* root);

int main()
{
    // Initialize window
    InitializeWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "maze");
    
    // Initialize camera
    Camera camera = InitializeCamera();
    
    // Set up maze data
    Root root = {0}; // Initialize all struct fields to zero
    const int mazeWidth = MAZE_SIZE;
    const int mazeHeight = MAZE_SIZE;
    Cell **path = InitializeMazeData(mazeHeight, mazeWidth, &root);
    
    if (path == NULL)
    {
        fprintf_s(stderr, "Memory allocation failed for path.\n");
        exit(EXIT_FAILURE);
    }
    
    // Load game resources
    GameResources resources = LoadGameResources(path, mazeHeight, mazeWidth);
    
    // Set cursor constraints
    DisableCursor();
    
    // Main game loop
    while (!WindowShouldClose())
    {
        // Update player movement
        Vector3 oldCamPos = camera.position;
        Vector3 oldCamTarget = camera.target;
        UpdatePlayerMovement(&camera, &oldCamPos, &oldCamTarget);
        
        // Calculate player position in grid
        Vector2 playerPos = { camera.position.x, camera.position.z };
        int playerCellX = (int)(playerPos.x - resources.mapPosition.x + 0.5f);
        int playerCellY = (int)(playerPos.y - resources.mapPosition.z + 0.5f);
        
        // Handle bounds checking
        playerCellX = Clamp(playerCellX, 0, resources.cubicmap.width - 1);
        playerCellY = Clamp(playerCellY, 0, resources.cubicmap.height - 1);
        
        // Handle collision detection
        HandleCollisions(&camera, oldCamPos, oldCamTarget, resources, playerCellX, playerCellY);
        
        // Render the frame
        RenderFrame(camera, resources, root, playerCellX, playerCellY);
    }
    
    // Cleanup resources
    CleanupResources(&resources, path, mazeHeight);
}

Cell** InitializeMazeData(int height, int width, Root* root)
{
    Cell **path = createMaze(height, width);
    if (path == NULL) return NULL;
    
    InitializeMaze(path, height, width);
    
    root->x = height - 1;
    root->y = width - 1;
    
    srand((unsigned int)clock());
    RandomizeMaze(path, height, width, root, height * width * 20);
    
    return path;
}