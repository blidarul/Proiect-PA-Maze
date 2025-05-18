#include "game.h"
#include "pause.h"
#include "title.h"

int main(void)
{
    InitGame();
    RunGameLoop();
    CleanupGame();
    
    return 0;
}