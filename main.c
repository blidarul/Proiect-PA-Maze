#include "game.h"

int main(void)
{
    InitGame();
    RunGameLoop();
    CleanupGame();
    
    return 0;
}