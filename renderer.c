#include "renderer.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "camera.h"

void InitializeWindow(int width, int height, const char* title)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, title);
    
    // Set window icon
    Image icon = LoadImage("resources/icon.png");
    if (icon.data == NULL) 
    {
        TraceLog(LOG_WARNING, "Failed to load window icon: resources/icon.png");
    }
    else
    {
        SetWindowIcon(icon);
        UnloadImage(icon);
    }
    
    SetTargetFPS(60);
}

void DrawMinimap(GameResources resources, int playerCellX, int playerCellY)
{
    // Draw minimap UI
    DrawTextureEx(resources.minimapTexture, 
                 (Vector2){ GetScreenWidth() - resources.minimapTexture.width*MINIMAP_SCALE - 
                           MINIMAP_DIST_FROM_BORDER, MINIMAP_DIST_FROM_BORDER }, 
                 0.0f, MINIMAP_SCALE, WHITE);
    
    // Draw player position on minimap
    DrawRectangle(GetScreenWidth() - resources.minimapTexture.width * MINIMAP_SCALE - 
                 MINIMAP_DIST_FROM_BORDER + playerCellX * MINIMAP_SCALE, 
                 MINIMAP_DIST_FROM_BORDER + playerCellY * MINIMAP_SCALE, 
                 MINIMAP_SCALE, MINIMAP_SCALE, PLAYER_COLOR);
    
    DrawFPS(10, 10);
}

void RenderFrame(Camera camera, GameResources resources, Root root, int playerCellX, int playerCellY)
{
    BeginDrawing();
    
    ClearBackground(RAYWHITE);
    
    // Draw 3D scene
    BeginMode3D(camera);
        DrawModel(resources.model, (Vector3) {0.5f, 0.0f, 0.5f}, 1.0f, WHITE);
    EndMode3D();
    
    // Draw UI elements
    DrawMinimap(resources, playerCellX, playerCellY);

    EndDrawing();
}