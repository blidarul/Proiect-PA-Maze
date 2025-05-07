#include "game.h"

// Private function prototypes
static Cell** InitializeMazeData(int height, int width, Root* root);

// Game state variables
static Camera camera;
static Root root;
static Cell **path;
static GameResources resources;
static const int mazeWidth = MAZE_SIZE;
static const int mazeHeight = MAZE_SIZE;

void InitGame(void)
{
    // Initialize window
    InitializeWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "maze");
    
    // Initialize camera
    camera = InitializeCamera();
    
    // Set up maze data
    root = (Root){0};
    path = InitializeMazeData(mazeHeight, mazeWidth, &root);
    
    if (path == NULL)
    {
        fprintf(stderr, "Memory allocation failed for path.\n");
        exit(EXIT_FAILURE);
    }
    
    // Load game resources
    resources = LoadGameResources(path, mazeHeight, mazeWidth);
    
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
                break;
                
            case GAME_STATE_PAUSE:
                // Handle pause state
                break;
                
            case GAME_STATE_GAMEOVER:
                // Handle game over state
                break;
        }
    }
}

void CleanupGame(void)
{
    // Cleanup resources
    CleanupResources(&resources, path, mazeHeight);
}

static Cell** InitializeMazeData(int height, int width, Root* root)
{
    Cell **path = createMaze(height, width);
    if (path == NULL) return NULL;
    
    InitializeMaze(path, height, width);
    
    root->x = height - 1;
    root->y = width - 1;
    
    srand((unsigned int)clock());
    RandomizeMaze(path, height, width, root, height * width * MAZE_SIZE);
    
    return path;
}