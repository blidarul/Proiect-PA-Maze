#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "sound.h"
#include "title.h" // Make sure title.h is included for app_settings

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
                {
                    Menu_Result title_action = update_menu(&titleMenu);

                    if (title_action == MENU_START)
                    {
                        SetGameState(GAME_STATE_GAMEPLAY);
                    }
                    else if (title_action == MENU_SETTINGS)
                    {
                        SetGameState(GAME_STATE_SETTINGS);
                    }
                    else if (title_action == MENU_EXIT)
                    {
                        CleanupGame();
                        CloseWindow();
                    }

                    // Draw the title menu only if we are still in the title state
                    if (GetGameState() == GAME_STATE_TITLE)
                    {
                        BeginDrawing();
                        ClearBackground(RAYWHITE);
                        if (titleMenu.active)
                        {
                            draw_menu(&titleMenu);
                        }
                        EndDrawing();
                    }
                }
                break;
                
            case GAME_STATE_SETTINGS:
                update_settings(&app_settings);
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    SetGameState(GAME_STATE_TITLE);
                    if (!titleMenu.active) titleMenu.active = true;
                }
                BeginDrawing();
                ClearBackground(RAYWHITE);
                draw_settings(&app_settings);
                DrawText("Press ESC to go back", 10, 10, 20, DARKGRAY);
                EndDrawing();
                break;

            case GAME_STATE_GAMEPLAY:
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    SetGameState(GAME_STATE_PAUSE);
                    InitializePauseControls(&pauseMenu, SCREEN_WIDTH, SCREEN_HEIGHT);
                }
                else
                {
                    UpdatePlayerMovement(&camera, resources);

                    // Calculate player position in grid
                    int playerPixelX = (int)floorf(camera.position.x);
                    int playerPixelY = (int)floorf(camera.position.z);

                    playerPixelX = Clamp(playerPixelX, 0, resources.cubicimage.width - 1);
                    playerPixelY = Clamp(playerPixelY, 0, resources.cubicimage.height - 1);
                         
                    VisitCell(maze, playerPixelX, playerPixelY, &resources);
                    
                    BeginDrawing();
                    ClearBackground(DARKBLUE);
                    RenderFrame(camera, resources, maze->root, playerPixelX, playerPixelY, false);
                    EndDrawing();
                }
                break;
                
            case GAME_STATE_PAUSE:
                UpdatePauseControls(&pauseMenu); 
                
                Menu_action pauseAction = pauseMenu.action; 
                if (pauseAction == MENU_ACTION_RESUME) {
                    SetGameState(GAME_STATE_GAMEPLAY);
                }
                else if (pauseAction == MENU_ACTION_SETTINGS)
                {
                    SetGameState(GAME_STATE_SETTINGS);
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
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Question State - Not Implemented", 20, 20, 20, DARKGRAY);
                EndDrawing();
                break;
        }
    }
}

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

    // Reset player/game specific states if any
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