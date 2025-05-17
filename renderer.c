#include "renderer.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "camera.h"

void InitializeWindow(int width, int height, const char *title)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, title);

    // Set window icon
    Image icon = LoadImage("resources/icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);
}

void DrawMinimap(GameResources resources, int playerCellX, int playerCellY)
{
    // Draw minimap UI
    DrawTextureEx(resources.minimapTexture,
                  (Vector2){GetScreenWidth() - resources.minimapTexture.width * MINIMAP_SCALE -
                                MINIMAP_DIST_FROM_BORDER,
                            MINIMAP_DIST_FROM_BORDER},
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
    DrawModel(resources.model, (Vector3){0.5f, 0.0f, 0.5f}, 1.0f, WHITE);
    EndMode3D();

    // Draw UI elements
    DrawMinimap(resources, playerCellX, playerCellY);
    
    // GuiLabel((Rectangle){600, 100, 1000, 400}, resources.questions[59].questionText);
    // GuiMessageBox((Rectangle){600, 100, 500, 200},
    //                                 "#191#Question", resources.questions[59].questionText, "A;B;C;D");
    // GuiLabel((Rectangle){600, 200, 1000, 400}, resources.questions[59].answersText[0]);
    // GuiLabel((Rectangle){600, 300, 1000, 400}, resources.questions[59].answersText[1]);
    GuiWindowBox((Rectangle){500, 250, 600, 400}, 
                  "#191#Question");

    // Define spacing for answers
    float answerSpacing = 50; // Vertical spacing between answers
    float answerWidth = 600 - 40; // Leave padding on the sides
    float answerHeight = 40; // Height of each answer box
    float answerX = 500 + 20; // Padding from the left
    float firstAnswerY = 250 + 100; // Start below the question text

    // Draw each answer
    GuiLabel((Rectangle){answerX, firstAnswerY - answerSpacing, answerWidth, answerHeight}, resources.questions[59].questionText);
    GuiLabel((Rectangle){answerX, firstAnswerY, answerWidth, answerHeight}, resources.questions[59].answersText[0]);
    GuiLabel((Rectangle){answerX, firstAnswerY + answerSpacing, answerWidth, answerHeight}, resources.questions[59].answersText[1]);
    GuiLabel((Rectangle){answerX, firstAnswerY + 2 * answerSpacing, answerWidth, answerHeight}, resources.questions[59].answersText[2]);
    GuiLabel((Rectangle){answerX, firstAnswerY + 3 * answerSpacing, answerWidth, answerHeight}, resources.questions[59].answersText[3]);
    // if (showMessageBox)
    // {
    //     int result = GuiMessageBox((Rectangle){85, 70, 250, 100},
    //                                "#191#Message Box", "Hi! This is a message!", "Nice;Cool");

    //     if (result >= 0)
    //         showMessageBox = false;
    // }

    EndDrawing();
}