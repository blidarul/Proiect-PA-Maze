#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "sound.h"
#include "questions.h"
#include "config.h"

// Private function prototypes
static Maze* InitializeMazeData(int height, int width);
static void VisitCell(Maze *maze, int playerCellX, int playerCellY, GameResources *resources);

// Game state variables
static Camera camera;
static Maze *maze;
static GameResources resources;
static const int mazeWidth = MAZE_SIZE;
static const int mazeHeight = MAZE_SIZE;

// Menu structures
static Menu titleMenu;
static Pause_menu pauseMenu;

// Question state variables
static float questionTimer = 0.0f; // Timer to track elapsed time
static const float questionInterval = 10.0f; // Time interval (in seconds) for question pop-up
static RenderTexture2D lastFrameTexture; // RenderTexture to store the last 3D frame
bool isQuestionActive = false; // Tracks the number of questions answered
static int currentQuestionIndex = -1; // Stores the index of the current question
static Sound questionPopupSound; // Added for the question pop-up sound
static Sound correctAnswerSound;   // Sound for correct answer
static Sound incorrectAnswerSound; // Sound for incorrect answer
static Sound winSound;

static GameState previousGameState; // Added to store the state before settings

void InitGame(void)
{
    // Initialize window
    InitializeWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "maze");
    SetExitKey(KEY_NULL);
    
    // Initialize audio device
    InitAudioDevice();
    InitStepSounds("resources/Sound");

    // Initialize music & sounds
    InitBGM("resources/Sound/music.wav");
    questionPopupSound = LoadSound("resources/Sound/question_pop-up.wav"); // Load the question pop-up sound
    correctAnswerSound = LoadSound("resources/Sound/correct_answer.wav");     // Load correct answer sound
    incorrectAnswerSound = LoadSound("resources/Sound/wrong_answer.wav"); // Load incorrect answer sound
    winSound = LoadSound("resources/Sound/win_sound.wav");

    // Initialize game state system
    InitGameState();

    // Initialize Title Menu
    initialize_menu(&titleMenu, SCREEN_WIDTH, SCREEN_HEIGHT);
    initialize_settings(&app_settings, SCREEN_WIDTH, SCREEN_HEIGHT); // Initialize settings data

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

    // Initialize the render texture for capturing the last frame
    lastFrameTexture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    SetGameState(GAME_STATE_TITLE);
    previousGameState = GAME_STATE_TITLE; // Initialize previousGameState
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
                        previousGameState = GAME_STATE_TITLE; // Store current state
                        SetGameState(GAME_STATE_SETTINGS);
                    }
                    else if (title_action == MENU_EXIT)
                    {
                        CleanupGame();
                        CloseWindow();
                        return; // Exit RunGameLoop after cleanup
                    }

                    // Draw the title menu only if we are still in the title state
                    if (GetGameState() == GAME_STATE_TITLE)
                    {
                        BeginDrawing();
                        ClearBackground(BEIGE);
                        if (titleMenu.active)
                        {
                            draw_menu(&titleMenu);
                        }
                        EndDrawing();
                    }
                }
                break;
                
            case GAME_STATE_SETTINGS:
                {
                    bool shouldExitSettings = update_settings(&app_settings);
                    if (shouldExitSettings)
                    {
                        SetGameState(previousGameState); // Go back to the stored previous state
                        // Ensure title menu is active if returning to title
                        if (previousGameState == GAME_STATE_TITLE && !titleMenu.active)
                        {
                            titleMenu.active = true;
                        }
                    }
                    BeginDrawing();
                    ClearBackground(BEIGE);
                    draw_settings(&app_settings);
                    EndDrawing();
                }
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

                    // --- Check for WIN CONDITION ---
                    if (playerPixelX == (mazeWidth*2-1) && playerPixelY == (mazeHeight*2-1))
                    {
                        SetGameState(GAME_STATE_WIN);
                        PlaySound(winSound);
                    }
                    // --- End of WIN CONDITION check ---


                    // Update the question timer
                    questionTimer += GetFrameTime();
                    if (questionTimer >= questionInterval && GetGameState() == GAME_STATE_GAMEPLAY) // Check if still in gameplay
                    {
                        PlaySound(questionPopupSound); 
                        BeginTextureMode(lastFrameTexture);
                        ClearBackground(DARKBLUE);
                        RenderFrame(camera, resources, maze->root, playerPixelX, playerPixelY, false);
                        EndTextureMode();

                        currentQuestionIndex = rand() % 60; 
                        questionTimer = 0.0f; 
                        SetGameState(GAME_STATE_QUESTION);
                    }
                    
                    // Only draw gameplay if not transitioning to another state this frame
                    if (GetGameState() == GAME_STATE_GAMEPLAY) 
                    {
                        BeginDrawing();
                        ClearBackground(DARKBLUE);
                        RenderFrame(camera, resources, maze->root, playerPixelX, playerPixelY, false);
                        EndDrawing();
                    }
                }
                break;

            case GAME_STATE_PAUSE:
                { // Added braces for scope
                    Menu_action pauseAction = UpdatePauseControls(&pauseMenu); // UpdatePauseControls should return the action

                    if (pauseAction == MENU_ACTION_RESUME)
                    {
                        SetGameState(GAME_STATE_GAMEPLAY);
                    }
                    else if (pauseAction == MENU_ACTION_SETTINGS)
                    {
                        previousGameState = GAME_STATE_PAUSE; // Store current state
                        SetGameState(GAME_STATE_SETTINGS);
                    }
                    else if (pauseAction == MENU_ACTION_EXIT)
                    {
                        CleanupGame(); // Ensure cleanup before closing
                        CloseWindow();
                        return; // Exit RunGameLoop after cleanup
                    }

                    BeginDrawing();
                    ClearBackground(RAYWHITE); // Or draw the game screen blurred
                    RenderFrame(camera, resources, maze->root,
                                (int)(camera.position.x), (int)(camera.position.z),
                                true); // Assuming true means draw blurred or dimmed for pause
                    DrawPauseControls(&pauseMenu);
                    EndDrawing();
                }
                break;

            case GAME_STATE_QUESTION:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                // Draw the last frame as the background
                DrawTextureRec(lastFrameTexture.texture,
                               (Rectangle){0, 0, (float)lastFrameTexture.texture.width, (float)-lastFrameTexture.texture.height},
                               (Vector2){0, 0}, WHITE);

                // Draw the question window with resources parameter
                DrawQuestionWindow(resources.questions, currentQuestionIndex, correctAnswerSound, incorrectAnswerSound, &resources);
                EndDrawing();
                break;

            case GAME_STATE_WIN:
                {
                    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        // Reset game or relevant parts before going to title
                        // For example, re-initialize maze and player position if needed for a new game
                        // This might involve calling parts of InitGame() or a dedicated ResetGame() function.
                        // For now, just go back to title.
                        SetGameState(GAME_STATE_TITLE);
                        // Ensure title menu is active
                        if (!titleMenu.active) titleMenu.active = true; 
                    }

                    BeginDrawing();
                    ClearBackground(BEIGE); // Or any color you like for the win screen

                    const char* winText = "YOU WIN!";
                    int winTextWidth = MeasureText(winText, 60);
                    DrawText(winText, GetScreenWidth()/2 - winTextWidth/2, GetScreenHeight()/2 - 60, 60, GOLD);

                    const char* instructionText = "Press ENTER or Click to return to Title Screen";
                    int instructionTextWidth = MeasureText(instructionText, 20);
                    DrawText(instructionText, GetScreenWidth()/2 - instructionTextWidth/2, GetScreenHeight()/2 + 20, 20, DARKGRAY);
                    
                    EndDrawing();
                }
                break;
        }
    }
    // If loop exits due to WindowShouldClose() but not MENU_EXIT
    CleanupGame(); 
}

void CleanupGame(void)
{
    // Cleanup resources
    CleanupResources(&resources, maze, mazeHeight);
    
    unload_menu(&titleMenu);
    UnloadPauseControls(&pauseMenu);

    UnloadStepSounds();     
    UnloadBGM();
    UnloadSound(questionPopupSound); // Unload the question pop-up sound
    UnloadSound(correctAnswerSound);   // Unload correct answer sound
    UnloadSound(incorrectAnswerSound); // Unload incorrect answer sound
    CloseAudioDevice();

    // Unload the render texture
    UnloadRenderTexture(lastFrameTexture);
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