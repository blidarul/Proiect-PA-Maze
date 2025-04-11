#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "maze.h"

typedef struct {
    int x;
    int y;
    int facing;
} PLAYER;

// Player movement speed constant (pixels per second)
#define PLAYER_SPEED(scale) (scale * 5.0f)

// Check and handle player collisions with walls
void HandlePlayerBorderCollisions(PLAYER *player, int wallThickness, int screenWidth, int screenHeight, int playerRadius,Cell playerCell,int scale);

// Update player position based on input
void UpdatePlayer(PLAYER *player, int scale, int wallThickness, int screenWidth, int screenHeight, int playerRadius,Cell playerCell);

// Draw player on screen
void DrawPlayer(PLAYER *player, int radius, Color color);

#endif