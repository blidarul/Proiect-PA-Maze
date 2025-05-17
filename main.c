#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
//#include "raylib.h"
//#include "raymath.h"
#include "maze.h"
#include "camera.h"
#include "renderer.h"
#include "gameResources.h"
#include "config.h"
#include "game.h"
#include "pause.h"
#include "title.h"

int main(void)
{
    Game *game;
    InitGame(&game);
    RunGameLoop(&game);
    CleanupGame(&game);
    
    return 0;
}