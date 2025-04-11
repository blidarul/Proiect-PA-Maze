#include "player.h"

void HandlePlayerBorderCollisions(PLAYER *player, int wallThickness, int screenWidth, int screenHeight, int playerRadius, Cell playerCell1,int scale) {
    if(player->x < wallThickness) 
        player->x = wallThickness;
    if(player->x + playerRadius > screenWidth - wallThickness) 
        player->x = screenWidth - playerRadius - wallThickness;
    if(player->y < wallThickness) 
        player->y = wallThickness;
    if(player->y + playerRadius > screenHeight - wallThickness) 
        player->y = screenHeight - playerRadius - wallThickness;
    unsigned int pEast,pNorth,pWest,pSouth;
    getDir(playerCell1, &pEast, &pNorth, &pWest, &pSouth);
    if(pNorth == NO_PATH && player->y < player->y /scale * scale + wallThickness) 
        player->y = player->y /scale * scale + wallThickness;
    if(pWest == NO_PATH && player->x < player->x /scale * scale + wallThickness)
        player->x = player->x /scale * scale + wallThickness;
    if(pEast == NO_PATH && player->x + playerRadius > player->x /scale * scale + scale) 
        player->x = player->x /scale * scale + scale - playerRadius;
    if(pSouth == NO_PATH && player->y + playerRadius > player->y /scale * scale + scale)
        player->y = player->y /scale * scale + scale - playerRadius;
}

void UpdatePlayer(PLAYER *player, int scale, int wallThickness, int screenWidth, int screenHeight, int playerRadius,Cell playerCell) {
    // Get delta time in seconds
    float deltaTime = GetFrameTime();
    float moveSpeed = PLAYER_SPEED(scale) * deltaTime;

    if (IsKeyDown(KEY_D)) {
        if(IsKeyDown(KEY_W))
        {
            player->x += moveSpeed /1.4142;
            player->y -= moveSpeed /1.4142;
        }
        else
        if(IsKeyDown(KEY_S))
        {
            player->x += moveSpeed / 1.4142;
            player->y += moveSpeed / 1.4142;
        }
        else
        player->x += moveSpeed;
    }
    else
    if (IsKeyDown(KEY_A)) {
        if(IsKeyDown(KEY_W))
        {
            player->x -= moveSpeed /1.4142;
            player->y -= moveSpeed /1.4142;
        }
        else
        if(IsKeyDown(KEY_S))
        {
            player->x -= moveSpeed / 1.4142;
            player->y += moveSpeed / 1.4142;
        }
        else
        player->x -= moveSpeed;
    }
    else
    if (IsKeyDown(KEY_W)) {
        player->y -= moveSpeed;
    }
    else
    if (IsKeyDown(KEY_S)) {
        player->y += moveSpeed;
    }
    
    HandlePlayerBorderCollisions(player, wallThickness, screenWidth, screenHeight, playerRadius,playerCell,scale);
}

void DrawPlayer(PLAYER *player, int radius, Color color) {
    DrawRectangle(player->x, player->y, radius, radius, color);
}