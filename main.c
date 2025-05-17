#include "game.h"
#define RAYGUI_IMPLEMENTATION


int main(void)
{
    InitGame();
    RunGameLoop();
    CleanupGame();
    
    return 0;
}