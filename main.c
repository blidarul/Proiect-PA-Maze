#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "maze.h"
#include "camera.h"
#include "renderer.h"
#include "gameResources.h"
#include "config.h"
#include "game.h"

int main(void)
{
    InitGame();
    RunGameLoop();
    CleanupGame();
    
    return 0;
}