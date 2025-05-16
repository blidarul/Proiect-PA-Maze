#include "gameResources.h"
#include <stdio.h>
#include <string.h>

GameResources LoadGameResources(Maze *maze, int height, int width)
{
    GameResources resources = { 0 };
    FILE *questionText = fopen("resources/Questions.txt", "r");
    FILE *answerText = fopen("resources/Answers.txt", "r");
    char buffer[1024];
    int i = 0, j, index;
    while(fgets(buffer, MAX_QUESTION_SIZE, questionText)) {
        strcpy(resources.questions[i].questionText, buffer);
        i++;
    }
    i = 0;
    while(fgets(buffer, MAX_QUESTION_SIZE, answerText)) {
        index = 0;
        for(j = 0; j < strlen(buffer); j++) {
            if(buffer[j] >= 'a' && buffer[j] <= 'z') {
                resources.questions[i].answers[index] = buffer[j];
                index++;
            }
        }
        resources.questions[i].answers[index] = '\0';
        // reading all the possible answers into the question
        fgets(buffer, MAX_QUESTION_SIZE, answerText);
        strcpy(resources.questions[i].answersText[0], buffer);
        fgets(buffer, MAX_QUESTION_SIZE, answerText);
        strcpy(resources.questions[i].answersText[1], buffer);
        fgets(buffer, MAX_QUESTION_SIZE, answerText);
        strcpy(resources.questions[i].answersText[2], buffer);
        fgets(buffer, MAX_QUESTION_SIZE, answerText);
        strcpy(resources.questions[i].answersText[3], buffer);
        i++;
    }
    resources.minimap = GenImageColor(width * 2 + 1, height * 2 + 1, DARKGRAY);
    // Generate images
    resources.cubicimage = ConvertMazeToCubicMap(maze, height, width);
    
    if (resources.cubicimage.data == NULL)
    {
        fprintf(stderr, "Failed to convert maze to cubic map\n");
        return resources; // Return empty resources
    }
    
    // Debug exports
    if (!ExportImage(resources.cubicimage, "resources/cubicmap.png")) 
    {
        fprintf(stderr, "Warning: Failed to export cubicmap.png\n");
    }
    
    if (!ExportImage(resources.minimap, "resources/minimap.png"))
    {
        fprintf(stderr, "Warning: Failed to export minimap.png\n");
    }
    
    // Load textures
    resources.cubicmap = LoadTextureFromImage(resources.cubicimage);
    resources.minimapTexture = LoadTextureFromImage(resources.minimap);
    resources.mesh = GenMeshCubicmap(resources.cubicimage, (Vector3){ 1.0f, 1.0f, 1.0f });
    resources.model = LoadModelFromMesh(resources.mesh);
    
    resources.texture = LoadTexture("resources/cubicmap_atlas.png");
    if (resources.texture.id == 0)
    {
        fprintf(stderr, "Failed to load cubicmap_atlas.png\n");
    }
    
    resources.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = resources.texture;
    
    // Load collision data
    resources.mapPixels = LoadImageColors(resources.cubicimage);
    
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