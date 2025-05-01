#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "maze.h"

// Constants
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define MAZE_SIZE 20
#define MINIMAP_SCALE 5.0f
#define PLAYER_COLOR RED
#define MINIMAP_DIST_FROM_BORDER 20.0f
#define PLAYER_RADIUS 0.1f

// Global structure to hold game resources
typedef struct
{
    Model model;
    Mesh mesh;
    Texture2D texture;
    Texture2D cubicmap;
    Texture2D minimapTexture;
    Color *mapPixels;
    Vector3 mapPosition;
} GameResources;

// Function prototypes
void InitializeWindow(int width, int height, const char* title);
Camera InitializeCamera(void);
Cell** InitializeMazeData(int height, int width, Root* root);
GameResources LoadGameResources(Cell** path, int height, int width);
void UnloadGameResources(GameResources* resources);
void UpdatePlayerMovement(Camera* camera, Vector3* oldCamPos, Vector3* oldCamTarget);
void HandleCollisions(Camera* camera, Vector3 oldCamPos, Vector3 oldCamTarget, GameResources resources, 
                     int playerCellX, int playerCellY);
void RenderFrame(Camera camera, GameResources resources, Root root, int playerCellX, int playerCellY);
void CleanupResources(GameResources* resources, Cell** path, int height);

int main()
{
    // Initialize window
    InitializeWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "maze");
    
    // Initialize camera
    Camera camera = InitializeCamera();
    
    // Set up maze data
    Root root;
    const int mazeWidth = MAZE_SIZE;
    const int mazeHeight = MAZE_SIZE;
    Cell **path = InitializeMazeData(mazeHeight, mazeWidth, &root);
    
    if (path == NULL)
    {
        fprintf(stderr, "Memory allocation failed for path.\n");
        return -1;
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
    
    return 0;
}

// Initialize the window and OpenGL context
void InitializeWindow(int width, int height, const char* title)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, title);
    SetTargetFPS(60);
}

// Initialize the camera with default settings
Camera InitializeCamera(void)
{
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.2f, 0.4f, 0.2f };
    camera.target = (Vector3){ 0.185f, 0.4f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    return camera;
}

// Initialize and set up the maze data
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

// Load all game resources (textures, models, etc.)
GameResources LoadGameResources(Cell** path, int height, int width)
{
    GameResources resources = { 0 };
    
    // Generate images
    Image minimap = { 0 };
    Image mazeImage = ConvertMazeToCubicMap(path, height, width, &minimap);
    
    // Debug exports
    ExportImage(mazeImage, "resources/cubicmap.png");
    ExportImage(minimap, "resources/minimap.png");
    
    // Load textures
    resources.cubicmap = LoadTextureFromImage(mazeImage);
    resources.minimapTexture = LoadTextureFromImage(minimap);
    resources.mesh = GenMeshCubicmap(mazeImage, (Vector3){ 1.0f, 1.0f, 1.0f });
    resources.model = LoadModelFromMesh(resources.mesh);
    
    resources.texture = LoadTexture("resources/cubicmap_atlas.png");
    resources.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = resources.texture;
    
    // Load collision data
    resources.mapPixels = LoadImageColors(mazeImage);
    UnloadImage(mazeImage);
    
    resources.mapPosition = (Vector3){ -1.0f, 0.0f, -1.0f };
    
    return resources;
}

// Update player movement based on input - separate rotation from position
void UpdatePlayerMovement(Camera* camera, Vector3* oldCamPos, Vector3* oldCamTarget)
{
    // Store current position and target
    *oldCamPos = camera->position;
    *oldCamTarget = camera->target;
    
    // Update camera (handles input)
    UpdateCamera(camera, CAMERA_FIRST_PERSON);
}

// Handle player collisions with walls
void HandleCollisions(Camera* camera, Vector3 oldCamPos, Vector3 oldCamTarget, GameResources resources, 
                     int playerCellX, int playerCellY)
{
    // Calculate position delta
    Vector3 positionDelta = {
        camera->position.x - oldCamPos.x,
        camera->position.y - oldCamPos.y,
        camera->position.z - oldCamPos.z
    };
    
    // Calculate direction change separately
    Vector3 oldDirection = {
        oldCamTarget.x - oldCamPos.x,
        oldCamTarget.y - oldCamPos.y,
        oldCamTarget.z - oldCamPos.z
    };
    
    Vector3 newDirection = {
        camera->target.x - camera->position.x,
        camera->target.y - camera->position.y,
        camera->target.z - camera->position.z
    };
    
    // Increase precision for collision testing
    const int numSteps = 5;  // Increased from 3 to 5
    bool collision = false;
    Vector2 wallNormal = {0};
    
    // First check if final position is in a wall - important safety check
    Vector2 finalPlayerPos = { camera->position.x, camera->position.z };
    
    // Then check intermediate positions for smoother collision detection
    for (int step = 1; step <= numSteps && !collision; step++) {
        // Test at intermediate positions
        float t = (float)step / numSteps;
        Vector3 testPos = {
            oldCamPos.x + positionDelta.x * t,
            oldCamPos.y + positionDelta.y * t, 
            oldCamPos.z + positionDelta.z * t
        };
        
        Vector2 playerPos = { testPos.x, testPos.z };
        
        // Slightly larger radius to prevent slipping through walls
        const float extendedRadius = PLAYER_RADIUS * 1.2f;
        
        // Check surrounding cells in a wider area (increased from 2 to 3)
        for (int y = playerCellY - 3; y <= playerCellY + 3 && y < resources.cubicmap.height && !collision; y++) {
            for (int x = playerCellX - 3; x <= playerCellX + 3 && x < resources.cubicmap.width && !collision; x++) {
                if (y < 0 || x < 0) continue;
                
                if ((resources.mapPixels[y*resources.cubicmap.width + x].r == 255) &&
                    (CheckCollisionCircleRec(playerPos, extendedRadius,
                    (Rectangle){ resources.mapPosition.x - 0.5f + x*1.0f, 
                                resources.mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
                {
                    collision = true;
                    
                    // Calculate wall normal
                    float wallCenterX = resources.mapPosition.x - 0.5f + x*1.0f + 0.5f;
                    float wallCenterZ = resources.mapPosition.z - 0.5f + y*1.0f + 0.5f;
                    
                    float dx = playerPos.x - wallCenterX;
                    float dz = playerPos.y - wallCenterZ;
                    
                    if (fabs(dx) > fabs(dz)) {
                        wallNormal.x = (dx > 0) ? 1.0f : -1.0f;
                        wallNormal.y = 0.0f;
                    } else {
                        wallNormal.x = 0.0f;
                        wallNormal.y = (dz > 0) ? 1.0f : -1.0f;
                    }
                }
            }
        }
    }
    
    if (collision)
    {
        // Calculate movement vector
        Vector2 movement = { 
            positionDelta.x,
            positionDelta.z 
        };
        
        // Project movement along the wall
        float dotProduct = movement.x * wallNormal.x + movement.y * wallNormal.y;
        
        Vector2 perpendicular = {
            wallNormal.x * dotProduct,
            wallNormal.y * dotProduct
        };
        
        // Only the parallel component remains
        Vector2 parallel = {
            movement.x - perpendicular.x,
            movement.y - perpendicular.y
        };
        
        // Scale down the parallel movement slightly for safety
        parallel.x *= 0.9f;
        parallel.y *= 0.9f;
        
        // Apply rotation changes regardless of collision
        Vector3 rotationChange = {
            newDirection.x - oldDirection.x,
            newDirection.y - oldDirection.y,
            newDirection.z - oldDirection.z
        };
        
        // Reset position to old position
        camera->position = oldCamPos;
        
        // Apply the parallel movement (sliding along wall)
        camera->position.x += parallel.x;
        camera->position.z += parallel.y;
        
        // Update target based on our new position and the rotation change
        camera->target.x = camera->position.x + oldDirection.x + rotationChange.x;
        camera->target.y = camera->position.y + oldDirection.y + rotationChange.y;
        camera->target.z = camera->position.z + oldDirection.z + rotationChange.z;
        
        // Extra safety check - if our new position collides with any wall, retreat completely
        Vector2 newPos = { camera->position.x, camera->position.z };
        bool secondaryCollision = false;
        
        for (int y = playerCellY - 2; y <= playerCellY + 2 && y < resources.cubicmap.height; y++) {
            for (int x = playerCellX - 2; x <= playerCellX + 2 && x < resources.cubicmap.width; x++) {
                if (y < 0 || x < 0) continue;
                
                if ((resources.mapPixels[y*resources.cubicmap.width + x].r == 255) &&
                    (CheckCollisionCircleRec(newPos, PLAYER_RADIUS * 1.1f,
                    (Rectangle){ resources.mapPosition.x - 0.5f + x*1.0f, 
                                resources.mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
                {
                    secondaryCollision = true;
                    break;
                }
            }
            if (secondaryCollision) break;
        }
        
        // If still in wall, revert to original position completely
        if (secondaryCollision) {
            camera->position = oldCamPos;
        }
    }
}

// Render the frame with 3D elements and UI
void RenderFrame(Camera camera, GameResources resources, Root root, int playerCellX, int playerCellY)
{
    BeginDrawing();
    
    ClearBackground(RAYWHITE);
    
    // Draw 3D scene
    BeginMode3D(camera);
        DrawModel(resources.model, resources.mapPosition, 1.0f, WHITE);
    EndMode3D();
    
    // Draw minimap UI
    DrawTextureEx(resources.minimapTexture, 
                 (Vector2){ GetScreenWidth() - resources.minimapTexture.width*MINIMAP_SCALE - 
                           MINIMAP_DIST_FROM_BORDER, MINIMAP_DIST_FROM_BORDER }, 
                 0.0f, MINIMAP_SCALE, WHITE);
    
    // Draw player position on minimap
    DrawRectangle(GetScreenWidth() - resources.minimapTexture.width*MINIMAP_SCALE - 
                 MINIMAP_DIST_FROM_BORDER + playerCellX*MINIMAP_SCALE, 
                 MINIMAP_DIST_FROM_BORDER + playerCellY*MINIMAP_SCALE, 
                 MINIMAP_SCALE, MINIMAP_SCALE, PLAYER_COLOR);
    
    DrawFPS(10, 10);
    
    EndDrawing();
}

// Unload game resources
void UnloadGameResources(GameResources* resources)
{
    UnloadImageColors(resources->mapPixels);
    UnloadTexture(resources->cubicmap);
    UnloadTexture(resources->texture);
    UnloadTexture(resources->minimapTexture);
    UnloadModel(resources->model);
    UnloadMesh(resources->mesh);
}

// Cleanup all resources
void CleanupResources(GameResources* resources, Cell** path, int height)
{
    UnloadGameResources(resources);
    CloseWindow();
    
    // Free maze data
    for (int i = 0; i < height; i++)
    {
        if (path[i] != NULL)
        {
            free(path[i]);
        }
    }
    free(path);
}