#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "raylib.h"
#include "maze.h"
#include "player.h"


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAZE_SIZE 20


int main()
{
    
    // Initialize the window and OpenGL context
    //_________________________________________________________________________
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;
    
    InitWindow(screenWidth, screenHeight, "maze");
    SetTargetFPS(120);
    //--------------------------------------------------------------------
    
    // Define the camera to look into our 3d world
    //_________________________________________________________________________
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.2f, 0.4f, 0.2f };    // Camera position
    camera.target = (Vector3){ 0.185f, 0.4f, 0.0f };    // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    //--------------------------------------------------------------------
    
    const int mazeWidth = MAZE_SIZE;
    const int mazeHeight = MAZE_SIZE;
    
    Cell **path = createMaze(mazeHeight, mazeWidth);
    if (path == NULL)
    {
        fprintf(stderr, "Memory allocation failed for path.\n");
        return -1;
    }
    
    InitializeMaze(path, mazeHeight, mazeWidth);
    
    Root root = {.x = mazeHeight - 1, .y = mazeWidth - 1};

    srand((unsigned int)clock());
    RandomizeMaze(path, mazeHeight, mazeWidth, &root, mazeHeight * mazeWidth * 20);

    //Generate the image coresponding to the maze and create the cubic map and texture
    //_________________________________________________________________________
    Image mazeImage = ConvertMazeToImage(path, mazeHeight, mazeWidth, root);
    
    ExportImage(mazeImage, "resources/maze.png");     // Export maze image to file (for debugging purposes)

    Texture2D cubicmap = LoadTextureFromImage(mazeImage);       // Convert image to texture to display (VRAM)
    Mesh mesh = GenMeshCubicmap(mazeImage, (Vector3){ 1.0f, 1.0f, 1.0f });
    Model model = LoadModelFromMesh(mesh);
    //--------------------------------------------------------------------

    // Load maze texture and set it to the model material
    //_________________________________________________________________________
    Texture2D texture = LoadTexture("resources/cubicmap_atlas.png");    // Load map texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;    // Set map diffuse texture
    //--------------------------------------------------------------------


    // Get map image data to be used for collision detection
    Color *mapPixels = LoadImageColors(mazeImage); // Load image data (RAM)
    UnloadImage(mazeImage);             // Unload image from RAM

    Vector3 mapPosition = { -1.0f, 0.0f, -1.0f };  // Set model position

    DisableCursor();                // Limit cursor to relative movement inside the window

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        Vector3 oldCamPos = camera.position;    // Store old camera position

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // Check player collision (we simplify to 2D collision detection)
        Vector2 playerPos = { camera.position.x, camera.position.z };
        float playerRadius = 0.1f;  // Collision radius (player is modelled as a cilinder for collision)

        int playerCellX = (int)(playerPos.x - mapPosition.x + 0.5f);
        int playerCellY = (int)(playerPos.y - mapPosition.z + 0.5f);

        // Out-of-limits security check
        if (playerCellX < 0) playerCellX = 0;
        else if (playerCellX >= cubicmap.width) playerCellX = cubicmap.width - 1;

        if (playerCellY < 0) playerCellY = 0;
        else if (playerCellY >= cubicmap.height) playerCellY = cubicmap.height - 1;

        // Check map collisions using image data and player position
        // TODO: Improvement: Just check player surrounding cells for collision
        for (int y = 0; y < cubicmap.height; y++)
        {
            for (int x = 0; x < cubicmap.width; x++)
            {
                if ((mapPixels[y*cubicmap.width + x].r == 255) &&       // Collision: white pixel, only check R channel
                    (CheckCollisionCircleRec(playerPos, playerRadius,
                    (Rectangle){ mapPosition.x - 0.5f + x*1.0f, mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
                {
                    // Collision detected, reset camera position
                    camera.position = oldCamPos;
                }
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                DrawModel(model, mapPosition, 1.0f, WHITE);                     // Draw maze map
            EndMode3D();

            DrawTextureEx(cubicmap, (Vector2){ GetScreenWidth() - cubicmap.width*4.0f - 20, 20.0f }, 0.0f, 4.0f, WHITE);
            DrawRectangleLines(GetScreenWidth() - cubicmap.width*4 - 20, 20, cubicmap.width*4, cubicmap.height*4, GREEN);

            // Draw player position radar
            DrawRectangle(GetScreenWidth() - cubicmap.width*4 - 20 + playerCellX*4, 20 + playerCellY*4, 4, 4, RED);

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    
    // De-Initialization
    //_________________________________________________________________________
    UnloadTexture(cubicmap);
    UnloadImage(mazeImage);
    CloseWindow(); // Close window and OpenGL context
    
    for (int i = 0; i < mazeHeight; i++)
    {
        if (path[i] != NULL)
        {
            free(path[i]);
        }
    }
    //--------------------------------------------------------------------

    return 0;
}