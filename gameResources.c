#include "gameResources.h"

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

void UnloadGameResources(GameResources* resources)
{
    UnloadImageColors(resources->mapPixels);
    UnloadTexture(resources->cubicmap);
    UnloadTexture(resources->texture);
    UnloadTexture(resources->minimapTexture);
    UnloadModel(resources->model);
    UnloadMesh(resources->mesh);
}

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