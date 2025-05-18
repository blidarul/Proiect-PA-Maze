#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "raygui.h"

// Private function prototypes
static Maze *InitializeMazeData(int height, int width);
static void VisitCell(Maze *maze, int playerCellX, int playerCellY, GameResources *resources);

// Game state variables
static Camera camera;
static Maze *maze;
static GameResources resources;
static const int mazeWidth = MAZE_SIZE;
static const int mazeHeight = MAZE_SIZE;
static RenderTexture2D lastFrameTexture;

void InitGame(void)
{
    // Initialize window
    InitializeWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "maze");

    // Initialize camera
    camera = InitializeCamera();

    // Initialize audio device
    InitAudioDevice();
    InitStepSounds("resources/Sound");

    // Initialize music
    InitBGM("resources/Sound/music.wav");

    maze = InitializeMazeData(mazeHeight, mazeWidth);

    if (maze == NULL)
    {
        fprintf(stderr, "Memory allocation failed for path.\n");
        exit(EXIT_FAILURE);
    }

    // Load game resources
    resources = LoadGameResources(maze, mazeHeight, mazeWidth);

    // Initialize the render texture for capturing the last frame
    lastFrameTexture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Set cursor constraints
    DisableCursor();

    // Initialize game state
    InitGameState();
    SetGameState(GAME_STATE_GAMEPLAY);
}

void RunGameLoop(void)
{
    // Main game loop
    while (!WindowShouldClose())
    {   
        switch (GetGameState())
        {
        case GAME_STATE_TITLE:
            // Handle title screen
            break;

        case GAME_STATE_GAMEPLAY:
            // Update player movement
            UpdatePlayerMovement(&camera, resources);

            // Calculate player position in grid
            Vector2 playerPos = {camera.position.x, camera.position.z};
            int playerCellX = (int)(playerPos.x);
            int playerCellY = (int)(playerPos.y);

            // Handle bounds checking
            playerCellX = Clamp(playerCellX, 0, resources.cubicmap.width - 1);
            playerCellY = Clamp(playerCellY, 0, resources.cubicmap.height - 1);

            VisitCell(maze, playerCellX, playerCellY, &resources);

            // Check if a question should be triggered
            if (maze->cellsVisited % 20 == 0 && maze->cellsVisited > 0 && !isQuestionActive)
            {
                // Capture the last frame
                BeginTextureMode(lastFrameTexture);
                ClearBackground(RAYWHITE);
                RenderFrame(camera, resources, maze->root, playerCellX, playerCellY, maze->cellsVisited);
                EndTextureMode();

                // Trigger question pop-up
                isQuestionActive = true;
                currentQuestionIndex = rand() % 60; // Randomize question index
                SetGameState(GAME_STATE_QUESTION);
            }

            UpdateStepSounds();
            RenderFrame(camera, resources, maze->root, playerCellX, playerCellY, maze->cellsVisited);
            break;

        case GAME_STATE_QUESTION:
            // Display the question window
            BeginDrawing();
            ClearBackground(RAYWHITE);

            // Draw the last frame as the background
            DrawTextureRec(lastFrameTexture.texture, 
                           (Rectangle){0, 0, lastFrameTexture.texture.width, -lastFrameTexture.texture.height}, 
                           (Vector2){0, 0}, WHITE);

            // Draw the question window
            DrawQuestionWindow(resources, currentQuestionIndex);
            EndDrawing();
            break;

        case GAME_STATE_PAUSE:
            // Handle pause state
            break;
        }

        UpdateBGM();
    }
}

void CleanupGame(void)
{
    // Cleanup resources
    CleanupResources(&resources, maze, mazeHeight);

    // Unload the render texture
    UnloadRenderTexture(lastFrameTexture);

    // Clean up loaded sounds
    UnloadStepSounds();
    CloseAudioDevice();
    UnloadBGM();
}

static Maze *InitializeMazeData(int height, int width)
{
    Maze *maze = createMaze(height, width);
    if (maze == NULL)
        return NULL;

    InitializeMaze(maze, height, width);

    maze->root.x = height - 1;
    maze->root.y = width - 1;

    srand((unsigned int)clock());
    RandomizeMaze(maze, height, width, height * width * MAZE_SIZE);

    return maze;
}

static void VisitCell(Maze *maze, int playerCellX, int playerCellY, GameResources *resources)
{
    // Convert from grid to maze coordinates for accessing path array
    int mazeX = playerCellX / 2;
    int mazeY = playerCellY / 2;

    if (!maze->path[mazeX][mazeY].visited)
    {
        maze->path[mazeX][mazeY].visited = true;
        maze->cellsVisited++;
        maze->cellsVisited++;
        RevealMinimap(maze, playerCellX, playerCellY, resources->cubicimage, &resources->minimap);
        UpdateMinimapTexture(resources);
    }
}