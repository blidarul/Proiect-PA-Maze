#include "gameResources.h"
#include <stdio.h>
#include <string.h>

GameResources LoadGameResources(Maze *maze, int mazeGridHeight, int mazeGridWidth)
{
    GameResources resources = { 0 };
    LoadQuestions(resources.questions);

    // Calculate minimap dimensions based on maze grid dimensions
    const int minimapImageWidth = mazeGridWidth * 2 + 1;
    const int minimapImageHeight = mazeGridHeight * 2 + 1;

    // Initialize minimap (e.g., with a dark gray background for unrevealed areas)
    resources.minimap = GenImageColor(minimapImageWidth, minimapImageHeight, DARKGRAY);

    // Draw a black border directly onto the minimap image
    // Top and bottom borders
    for (int x = 0; x < minimapImageWidth; x++)
    {
        ImageDrawPixel(&resources.minimap, x, 0, BLACK);                      // Top border
        ImageDrawPixel(&resources.minimap, x, minimapImageHeight - 1, BLACK); // Bottom border
    }
    // Left and right borders (excluding corners already drawn)
    for (int y = 1; y < minimapImageHeight - 1; y++)
    {
        ImageDrawPixel(&resources.minimap, 0, y, BLACK);                      // Left border
        ImageDrawPixel(&resources.minimap, minimapImageWidth - 1, y, BLACK); // Right border
    }

    // Generate cubicimage from maze data
    resources.cubicimage = ConvertMazeToCubicMap(maze, mazeGridHeight, mazeGridWidth);
    
    if (resources.cubicimage.data == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to generate cubicimage.");
    }

    // Load texture from minimap image
    resources.minimapTexture = LoadTextureFromImage(resources.minimap);
    
    resources.cubicmap = LoadTextureFromImage(resources.cubicimage);
    resources.mesh = GenMeshCubicmap(resources.cubicimage, (Vector3){ 1.0f, 1.0f, 1.0f });
    resources.model = LoadModelFromMesh(resources.mesh);
    
    resources.texture = LoadTexture("resources/textures/cubicmap_atlas.png");
    if (resources.texture.id == 0)
    {
        fprintf(stderr, "Failed to load cubicmap_atlas.png\n");
    }
    
    resources.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = resources.texture;
    
    // Load collision data
    resources.mapPixels = LoadImageColors(resources.cubicimage);
    
    return resources;
}

void UnloadGameResources(GameResources* resources)
{
    UnloadImageColors(resources->mapPixels);
    UnloadTexture(resources->cubicmap);
    UnloadTexture(resources->texture);
    UnloadTexture(resources->minimapTexture);
    UnloadModel(resources->model);
    UnloadMesh(resources->mesh);
    UnloadImage(resources->cubicimage);
    UnloadImage(resources->minimap);
}

void UpdateMinimapTexture(GameResources* resources)
{
    if (resources->minimap.data != NULL && resources->minimapTexture.id != 0)
    {
        UpdateTexture(resources->minimapTexture, resources->minimap.data);
    }
}

void CleanupResources(GameResources* resources, Maze *maze, int height)
{
    UnloadGameResources(resources);
    CloseWindow();
    
    if (maze != NULL)
    {
        for (int i = 0; i < height; i++)
        {
            free(maze->path[i]);
        }
        free(maze->path);
        free(maze);
    }
}