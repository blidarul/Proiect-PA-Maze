#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "raylib.h"
#include "maze.h"

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(unsigned char **path,int height,int width); // Update and draw one frame
static void DrawMaze(unsigned char **path,int height,int width);
void InitializeMaze(unsigned char **path, int mazeHeight, int mazeWidth);

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SCALE 80
#define WALL_THICKNESS (SCALE / 10)

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

    unsigned char **path = (unsigned char **)malloc(mazeHeight * sizeof(unsigned char *));
    if (path == NULL) {
        fprintf(stderr, "Memory allocation failed for path.\n");
        return -1;
    }
    for (int i = 0; i < mazeHeight; i++) {
        path[i] = (unsigned char *)malloc(mazeWidth * sizeof(unsigned char));
        if (path[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for path[%d].\n", i);
            for (int j = 0; j < i; j++) {
                free(path[j]);
            }
            free(path);
            path = NULL; // Avoid dangling pointer
            return -1;
        }
    }

    InitializeMaze(path, mazeHeight, mazeWidth);

    SetTargetFPS(10); // Set our game to run at 60 frames-per-second
    ROOT root = {.x = mazeHeight - 1,.y = mazeWidth - 1};
    root = RandomizeMaze(path,mazeHeight,mazeWidth,root.x,root.y,mazeHeight*mazeWidth*1000000);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateDrawFrame(path,mazeHeight,mazeWidth);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    for (int i = 0; i < mazeHeight; i++) {
        if (path[i] != NULL) {
            free(path[i]);
        }
    }
    if (path != NULL) {
        free(path);
    }
    return 0;
}

void InitializeMaze(unsigned char **path, int mazeHeight, int mazeWidth) {
    for (int i = 0; i < mazeHeight; i++) {
        for (int j = 0; j < mazeWidth; j++) {
            if (j == 0) {
                setDir(&path[i][j], OUTGOING_PATH, NO_PATH, NO_PATH, NO_PATH);
            } else if (j < mazeWidth - 1) {
                setDir(&path[i][j], OUTGOING_PATH, NO_PATH, INCOMING_PATH, NO_PATH);
            } else if (i == 0) {
                setDir(&path[i][j], NO_PATH, NO_PATH, INCOMING_PATH, OUTGOING_PATH);
            } else if (i < mazeHeight - 1) {
                setDir(&path[i][j], NO_PATH, INCOMING_PATH, INCOMING_PATH, OUTGOING_PATH);
            } else {
                setDir(&path[i][j], NO_PATH, INCOMING_PATH, INCOMING_PATH, NO_PATH);
            }
        }
    }
}

static void DrawMaze(unsigned char **path, int height, int width)
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned int east, north, west, south;
            getDir(path[i][j], &east, &north, &west, &south);
            int x = j * SCALE;
            int y = i * SCALE;

            // Draw top wall
            if (north == 0)
                DrawRectangle(x, y, SCALE, WALL_THICKNESS, BLUE);

            // Draw left wall
            if (west == 0)
                DrawRectangle(x, y, WALL_THICKNESS, SCALE, BLUE);

            // Draw bottom wall (only for the last row)
            if (i == height - 1 && south == 0)
                DrawRectangle(x, y + SCALE - WALL_THICKNESS, SCALE, WALL_THICKNESS, BLUE);

            // Draw right wall (only for the last column)
            if (j == width - 1 && east == 0)
                DrawRectangle(x + SCALE - WALL_THICKNESS, y, WALL_THICKNESS, SCALE, BLUE);

            // Draw corners to avoid gaps
            if (north == 0 && west == 0)
                DrawRectangle(x, y, WALL_THICKNESS, WALL_THICKNESS, BLUE); // Top-left corner

            if (north == 0 && east == 0)
                DrawRectangle(x + SCALE, y, WALL_THICKNESS, WALL_THICKNESS, BLUE); // Top-right corner

            if (south == 0 && west == 0)
                DrawRectangle(x, y + SCALE, WALL_THICKNESS, WALL_THICKNESS, BLUE); // Bottom-left corner

            if (south == 0 && east == 0)
                DrawRectangle(x + SCALE, y + SCALE, WALL_THICKNESS, WALL_THICKNESS, BLUE); // Bottom-right corner
        }
    }
}

// Update and draw game frame
static void UpdateDrawFrame(unsigned char **path,int height,int width)
{
    // Update
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawMaze(path,height,width);

    DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}