#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "sound.h"

// Private function prototypes
static Maze* InitializeMazeData(int height, int width);
static void VisitCell(Maze *maze, int playerCellX, int playerCellY, GameResources *resources);
static void RestartGameplay(void);

// Game state variables
static Camera camera;
static Maze *maze;
static GameResources resources;
static const int mazeWidth = MAZE_SIZE;
static const int mazeHeight = MAZE_SIZE;

// Menu structures
static Menu titleMenu;
static Pause_menu pauseMenu;

void InitGame(void)
{
    // Initialize window
    InitializeWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "maze");
    SetExitKey(KEY_NULL);
    
    // Initialize audio device
    InitAudioDevice();
    InitStepSounds("resources/Sound");

    // Initialize music
    InitBGM("resources/Sound/music.wav");

    // Initialize game state system
    InitGameState();

    // Initialize Title Menu
    initialize_menu(&titleMenu, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Initialize Camera, Maze, Resources for the first time (or after a full exit)
    camera = InitializeCamera();
    maze = InitializeMazeData(mazeHeight, mazeWidth);
    if (maze == NULL)
    {
        fprintf(stderr, "Memory allocation failed for maze.\n");
        CloseWindow();
        exit(EXIT_FAILURE);
    }
    resources = LoadGameResources(maze, mazeHeight, mazeWidth);
    
    SetGameState(GAME_STATE_TITLE);
}

void RunGameLoop(void)
{
    // Main game loop
    while (!WindowShouldClose())
    {
        UpdateBGM();

        switch (GetGameState())
        {
            case GAME_STATE_TITLE:
                update_menu(&titleMenu);

                if (!titleMenu.active)
                {
                    SetGameState(GAME_STATE_GAMEPLAY);
                }

                BeginDrawing();
                ClearBackground(RAYWHITE);
                draw_menu(&titleMenu);
                EndDrawing();
                break;
                
            case GAME_STATE_GAMEPLAY:
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P))
                {
                    SetGameState(GAME_STATE_PAUSE);
                    InitializePauseControls(&pauseMenu, SCREEN_WIDTH, SCREEN_HEIGHT);
                }

                UpdatePlayerMovement(&camera, resources);


                // Calculate player position in grid
                int playerPixelX = (int)floorf(camera.position.x);
                int playerPixelY = (int)floorf(camera.position.z);

                playerPixelX = Clamp(playerPixelX, 0, resources.cubicimage.width - 1);
                playerPixelY = Clamp(playerPixelY, 0, resources.cubicimage.height - 1);
                     

                VisitCell(maze, playerPixelX, playerPixelY, &resources);
                
                // Render the frame
                RenderFrame(camera, resources, maze->root, playerPixelX, playerPixelY);

                break;
                
            case GAME_STATE_PAUSE:
                // Handle pause menu updates and drawing
                UpdatePauseControls(&pauseMenu); // Example call
                
                // Determine action from pause menu
                Menu_action pauseAction = pauseMenu.action; // Assuming 'action' is updated by UpdatePauseControls
                if (pauseAction == MENU_ACTION_RESUME) {
                    SetGameState(GAME_STATE_GAMEPLAY);
                }
                else if (pauseAction == MENU_ACTION_RESTART)
                {
                    RestartGameplay();
                    SetGameState(GAME_STATE_GAMEPLAY);
                }
                else if (pauseAction == MENU_ACTION_EXIT)
                {
                    CloseWindow();
                }
                
                BeginDrawing();
                ClearBackground(RAYWHITE);
                RenderFrame(camera, resources, maze->root, 
                            (int)(camera.position.x), (int)(camera.position.z), 
                            true);
                DrawPauseControls(&pauseMenu);
                EndDrawing();
                break;

            case GAME_STATE_QUESTION:
                // Handle question state if you implement it
                BeginDrawing();
                ClearBackground(RAYWHITE);
                // Draw question UI
                DrawText("Question State - Not Implemented", 20, 20, 20, DARKGRAY);
                EndDrawing();
                break;
        }
    }
}

// Function to re-initialize game-specific elements for a restart
static void RestartGameplay(void)
{
    // Unload current game resources
    UnloadGameResources(&resources);

    for (int i = 0; i < mazeHeight; i++)
    {
        if (maze->path[i] != NULL)
        {
            free(maze->path[i]);
        }
    }
    if (maze->path != NULL) free(maze->path);
    if (maze != NULL) free(maze);

    // Re-initialize camera
    camera = InitializeCamera();
    
    // Re-initialize maze
    maze = InitializeMazeData(mazeHeight, mazeWidth);
    if (maze == NULL)
    {
        fprintf(stderr, "Memory allocation failed for maze on restart.\n");
        CloseWindow();
        exit(EXIT_FAILURE);
    }
    
    // Reload game resources
    resources = LoadGameResources(maze, mazeHeight, mazeWidth);

    // Reset player/game specific states if any (e.g., score, visited cells if not handled by maze re-init)
    maze->cellsVisited = 0;
}

void CleanupGame(void)
{
    // Cleanup resources
    CleanupResources(&resources, maze, mazeHeight);
    
    unload_menu(&titleMenu);
    UnloadPauseControls(&pauseMenu);

    UnloadStepSounds();     
    UnloadBGM();
    CloseAudioDevice();
}

static Maze* InitializeMazeData(int height, int width)
{
    Maze *newMaze = createMaze(height, width);
    if (newMaze == NULL) return NULL;
    
    InitializeMaze(newMaze, height, width);
    
    newMaze->root.x = height - 1;
    newMaze->root.y = width - 1;
    
    RandomizeMaze(newMaze, height, width, height * width * 10);
    
    newMaze->cellsVisited = 0;

    return newMaze;
}

static void VisitCell(Maze *mazeInstance, int playerCellX, int playerCellY, GameResources *res)
{
    if(playerCellX < 1 || playerCellY < 1 || 
       playerCellX >= res->cubicimage.width || 
       playerCellY >= res->cubicimage.height)
        return;
    
    int mazeX = (playerCellX - 1) / 2;
    int mazeY = (playerCellY - 1) / 2;
    
    if(mazeX < 0 || mazeY < 0 || 
       mazeX >= mazeWidth || mazeY >= mazeHeight)
        return;
    
    if(!mazeInstance->path[mazeX][mazeY].visited)
    {
        mazeInstance->path[mazeX][mazeY].visited = true;
        mazeInstance->cellsVisited++;
        RevealMinimap(mazeInstance, playerCellX, playerCellY, res->cubicimage, &res->minimap);
        UpdateMinimapTexture(res);
    }
}