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
#include "sound.h"
static bool isQuestionActive = false; // Tracks if a question is being displayed
static int currentQuestionIndex = -1; // Stores the index of the current question

// Initialize the game
void InitGame(void);

// Run the main game loop
void RunGameLoop(void);

// Clean up game resources
void CleanupGame(void);

#endif // GAME_H