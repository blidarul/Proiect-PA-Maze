#include <stdlib.h>

#include "raylib.h"
#include "maze.h"

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(unsigned char **path,int height,int width); // Update and draw one frame
static void DrawMaze(unsigned char **path,int height,int width);

const int SCALE = 80;

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "maze");

    const int mazeWidth = screenWidth / SCALE;
    const int mazeHeight = screenHeight / SCALE;

    unsigned char **path = (unsigned char **)malloc(mazeHeight * sizeof(unsigned char *));
    for (int i = 0; i < mazeHeight; i++)
        path[i] = (unsigned char *)malloc(mazeWidth * sizeof(unsigned char));

    // Basic Maze generation
    for (int i = 0; i < mazeHeight; i++)
        for (int j = 0; j < mazeWidth; j++)
        {
            // 00 no path
            // 10 outgoing path
            // 01 incoming path
            if (j == 0)
                setDir(&path[i][j], OUTGOING_PATH, NO_PATH, NO_PATH, NO_PATH);
                // path[i][j] = 0b10000000; //128
            else if (j < mazeWidth - 1)
                setDir(&path[i][j], OUTGOING_PATH, NO_PATH, INCOMING_PATH, NO_PATH);
                //path[i][j] = 0b10000100; // 132
            else if (i == 0)
                setDir(&path[i][j], NO_PATH, NO_PATH, INCOMING_PATH, OUTGOING_PATH);
                //path[i][j] = 0b00000110; // 6
            else if (i < mazeHeight - 1)
                setDir(&path[i][j], NO_PATH,INCOMING_PATH, INCOMING_PATH, OUTGOING_PATH);
                //path[i][j] = 0b00010110; // 22
            else
                setDir(&path[i][j], NO_PATH,INCOMING_PATH, INCOMING_PATH, NO_PATH);
        }

    SetTargetFPS(10); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateDrawFrame(path,mazeHeight,mazeWidth);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------


    for (int i = 0; i < mazeHeight; i++)
        free(path[i]);
    free(path);
    return 0;
}

static void DrawMaze(unsigned char **path,int height,int width)
{
    for(int i = 0; i < height; i++)
    for(int j = 0; j < width; j++)
    {
        unsigned int east,north,west,south;
        getDir(path[i][j],&east,&north,&west,&south);
        int x = j * SCALE;
        int y = i * SCALE;
        int wallThickness = SCALE / 10;
        if (north == 0) // Draw top wall
            DrawRectangle(x, y, SCALE, wallThickness, BLACK);
        if (west == 0) // Draw left wall
            DrawRectangle(x, y, wallThickness, SCALE, BLACK);
        if (south == 0) // Draw bottom wall
            DrawRectangle(x, y + SCALE - wallThickness, SCALE, wallThickness, BLACK);
        if (east == 0) // Draw right wall
            DrawRectangle(x + SCALE - wallThickness, y, wallThickness, SCALE, BLACK);
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