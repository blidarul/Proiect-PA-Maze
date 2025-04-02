#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "raylib.h"
#include "maze.h"



//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(Cell **path, int height, int width);
static void DrawMaze(Cell **path, int height, int width);

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SCALE 20
#define WALL_THICKNESS (SCALE / 10)
#define wallColor BLACK
#define pathColor RAYWHITE
#define playerRadius 9

int main()
{
    srand((unsigned int)clock());
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

    InitWindow(screenWidth, screenHeight, "maze");
    //--------------------------------------------------------------------------------------
    const int mazeWidth = screenWidth / SCALE;
    const int mazeHeight = screenHeight / SCALE;

    Cell **path = createMaze(mazeHeight, mazeWidth);
    if (path == NULL) {
        fprintf(stderr, "Memory allocation failed for path.\n");
        return -1;
    }

    InitializeMaze(path, mazeHeight, mazeWidth);

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    Root root = {.x = mazeHeight - 1, .y = mazeWidth - 1};
    root = RandomizeMaze(path, mazeHeight, mazeWidth, root.x, root.y, mazeHeight * mazeWidth * 20);

    // Main game loop
    
    PLAYER player = {10, 10};
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateDrawFrame(path, mazeHeight, mazeWidth);
        DrawRectangle(root.x, root.y, 9,9, RED);
        // Color *colors = LoadImageColors(screenImage);
        Image screenImage = LoadImageFromScreen();
        if (IsKeyDown(KEY_D)) {
            // if(ColorIsEqual(GetImageColor(screenImage,player.x + 9/2 + 1, player.y), wallColor))
            player.x += 1;
        }
        if (IsKeyDown(KEY_A)) player.x -= 1;
        if (IsKeyDown(KEY_W)) player.y -= 1;
        if (IsKeyDown(KEY_S)) player.y += 1;
        BeginDrawing();
            DrawRectangle(player.x, player.y, playerRadius, playerRadius, DARKGREEN);
        //EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    EndDrawing();
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    for (int i = 0; i < mazeHeight; i++) {
        if (path[i] != NULL) {
            free(path[i]);
        }
    }
    free(path);
    return 0;
}

static void DrawMaze(Cell **path, int height, int width)
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned int east, north, west, south;
            getDir(path[i][j], &east, &north, &west, &south);
            int x = j * SCALE;
            int y = i * SCALE;

            // Draw top wall
            if (north == 0)
                DrawRectangle(x, y, SCALE, WALL_THICKNESS, wallColor);

            // Draw left wall
            if (west == 0)
                DrawRectangle(x, y, WALL_THICKNESS, SCALE, wallColor);

            // Draw bottom wall (only for the last row)
            if (i == height - 1 && south == 0)
                DrawRectangle(x, y + SCALE - WALL_THICKNESS, SCALE, WALL_THICKNESS, wallColor);

            // Draw right wall (only for the last column)
            if (j == width - 1 && east == 0)
                DrawRectangle(x + SCALE - WALL_THICKNESS, y, WALL_THICKNESS, SCALE, wallColor);

            // Draw corners to avoid gaps
            if (north == 0 && west == 0)
                DrawRectangle(x, y, WALL_THICKNESS, WALL_THICKNESS, wallColor); // Top-left corner

            if (north == 0 && east == 0)
                DrawRectangle(x + SCALE, y, WALL_THICKNESS, WALL_THICKNESS, wallColor); // Top-right corner

            if (south == 0 && west == 0)
                DrawRectangle(x, y + SCALE, WALL_THICKNESS, WALL_THICKNESS, wallColor); // Bottom-left corner

            if (south == 0 && east == 0)
                DrawRectangle(x + SCALE, y + SCALE, WALL_THICKNESS, WALL_THICKNESS, wallColor); // Bottom-right corner
        }
    }
}

// Update and draw game frame
static void UpdateDrawFrame(Cell **path, int height, int width)
{
    BeginDrawing();
    ClearBackground(pathColor);
    DrawMaze(path, height, width);
    DrawFPS(10, 10);
    EndDrawing();
}