#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "config.h"
#include "camera.h"
#include "maze.h"
#include "renderer.h"
#include "gameResources.h"
#include "gameState.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "pause.h"
#include "title.h"

extern bool isQuestionActive; // Declare the variable as extern

// Initialize the game
void InitGame(void);

// Run the main game loop
void RunGameLoop(void);

// Clean up game resources
void CleanupGame(void);

#endif