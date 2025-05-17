#include "gameResources.h"
#include <stdio.h>


GameResources LoadGameResources(Maze *maze, int height, int width)
{
    GameResources resources = { 0 };
    LoadQuestions(resources.questions);
    resources.minimap = GenImageColor(width * 2 + 1, height * 2 + 1, DARKGRAY);
    // Generate images
    resources.cubicimage = ConvertMazeToCubicMap(maze, height, width);
    
    if (resources.cubicimage.data == NULL)
    {
        fprintf(stderr, "Failed to convert maze to cubic map\n");
        return resources; // Return empty resources
    }
    
    // Load textures
    resources.cubicmap = LoadTextureFromImage(resources.cubicimage);
    resources.minimapTexture = LoadTextureFromImage(resources.minimap);
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
    // Unload the previous texture first to prevent memory leak
    UnloadTexture(resources->minimapTexture);
    
    // Load the updated texture
    resources->minimapTexture = LoadTextureFromImage(resources->minimap);
}

void CleanupResources(GameResources* resources, Maze *maze, int height)
{
    UnloadGameResources(resources);
    CloseWindow();
    
    // Free maze data
    for (int i = 0; i < height; i++)
    {
        if (maze->path[i] != NULL)
        {
            free(maze->path[i]);
        }
    }
    free(maze->path);
    free(maze);
}