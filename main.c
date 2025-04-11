#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "raylib.h"
#include "maze.h"
#include "player.h"


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SCALE 100
#define WALL_THICKNESS (SCALE / 10)
#define WALL_COLOR DARKGRAY
#define PATH_COLOR RAYWHITE
#define PLAYER_RADIUS SCALE / 2

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(Cell **path, int height, int width, Root root, PLAYER *player);
static void DrawWalls(Cell **path, int height, int width);


int main()
{
    //random seed based on run-time
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

    SetTargetFPS(120); // Set our game to run at 60 frames-per-second
    Root root = {.x = mazeHeight - 1, .y = mazeWidth - 1};
    RandomizeMaze(path, mazeHeight, mazeWidth, &root, mazeHeight * mazeWidth * 20);

    // Main game loop
    
    PLAYER player = {.x = 10, .y = 10};
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateDrawFrame(path, mazeHeight, mazeWidth, root, &player);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    //EndDrawing();
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

static void DrawWalls(Cell **path, int height, int width)
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned int east, north, west, south;
            getDir(path[i][j], &east, &north, &west, &south);
            int x = j * SCALE;
            int y = i * SCALE;

            // Draw top wall
            if (north == 0)
                DrawRectangle(x, y, SCALE, WALL_THICKNESS, WALL_COLOR);

            // Draw left wall
            if (west == 0)
                DrawRectangle(x, y, WALL_THICKNESS, SCALE, WALL_COLOR);

            // Draw bottom wall (only for the last row)
            if (i == height - 1 && south == 0)
                DrawRectangle(x, y + SCALE - WALL_THICKNESS, SCALE, WALL_THICKNESS, WALL_COLOR);

            // Draw right wall (only for the last column)
            if (j == width - 1 && east == 0)
                DrawRectangle(x + SCALE - WALL_THICKNESS, y, WALL_THICKNESS, SCALE, WALL_COLOR);

            // Draw corners to avoid gaps
            if (north == 0 && west == 0)
                DrawRectangle(x, y, WALL_THICKNESS, WALL_THICKNESS, WALL_COLOR); // Top-left corner

            if (north == 0 && east == 0)
                DrawRectangle(x + SCALE, y, WALL_THICKNESS, WALL_THICKNESS, WALL_COLOR); // Top-right corner

            if (south == 0 && west == 0)
                DrawRectangle(x, y + SCALE, WALL_THICKNESS, WALL_THICKNESS, WALL_COLOR); // Bottom-left corner

            if (south == 0 && east == 0)
                DrawRectangle(x + SCALE, y + SCALE, WALL_THICKNESS, WALL_THICKNESS, WALL_COLOR); // Bottom-right corner
        }
    }
}

// Update and draw game frame
static void UpdateDrawFrame(Cell **path, int height, int width, Root root, PLAYER *player)
{
    // Update
    UpdatePlayer(player, SCALE, WALL_THICKNESS, SCREEN_WIDTH, SCREEN_HEIGHT, PLAYER_RADIUS,path[player->y / SCALE][player->x /SCALE]);
    bool win = false;
    if( (player->x + PLAYER_RADIUS > root.x * SCALE && player->x < (root.x + 1)*SCALE) &&(player->y + PLAYER_RADIUS> root.y * SCALE)&& player->y < (root.y + 1) *SCALE)
    {
        win = true;
    }

    // Draw
    BeginDrawing();
    ClearBackground(PATH_COLOR);
    DrawRectangle(root.x * SCALE, root.y * SCALE, SCALE, SCALE, RED);
    //DrawRectangle(root.x * SCALE, root.y * SCALE, SCALE, SCALE, BLUE);
    DrawWalls(path, height, width);
    DrawPlayer(player, PLAYER_RADIUS, DARKGREEN);
    if(win)
    {
        DrawText("You Win!", SCREEN_WIDTH / 2 - MeasureText("You Win!", 30) / 2, SCREEN_HEIGHT / 2 - 10, 30, GREEN);
    }
    else
    {
        DrawText("Keep Searching!", SCREEN_WIDTH / 2 - MeasureText("Keep Searching!", 30) / 2, SCREEN_HEIGHT / 2 - 10, 30, BLUE);
    }
    DrawFPS(10, 10);
    EndDrawing();
}