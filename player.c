#include "player.h"

void HandlePlayerBorderCollisions(PLAYER *player, int wallThickness, int screenWidth, int screenHeight, int playerRadius) {
    if(player->x < wallThickness) 
        player->x = wallThickness;
    if(player->x + playerRadius > screenWidth - wallThickness) 
        player->x = screenWidth - playerRadius - wallThickness;
    if(player->y < wallThickness) 
        player->y = wallThickness;
    if(player->y + playerRadius > screenHeight - wallThickness) 
        player->y = screenHeight - playerRadius - wallThickness;
}

void UpdatePlayer(PLAYER *player, int scale, int wallThickness, int screenWidth, int screenHeight, int playerRadius) {
    // Get delta time in seconds
    float deltaTime = GetFrameTime();
    float moveSpeed = PLAYER_SPEED(scale) * deltaTime;

    if (IsKeyDown(KEY_D)) {
        player->x += moveSpeed;
    }
    if (IsKeyDown(KEY_A)) {
        player->x -= moveSpeed;
    }
    if (IsKeyDown(KEY_W)) {
        player->y -= moveSpeed;
    }
    if (IsKeyDown(KEY_S)) {
        player->y += moveSpeed;
    }
    
    HandlePlayerBorderCollisions(player, wallThickness, screenWidth, screenHeight, playerRadius);
}

void DrawPlayer(PLAYER *player, int radius, Color color) {
    DrawRectangle(player->x, player->y, radius, radius, color);
}