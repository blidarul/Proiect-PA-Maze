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