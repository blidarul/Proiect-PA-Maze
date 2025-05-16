#include "game.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main(void)
{
    InitGame();
    RunGameLoop();
    CleanupGame();
    
    return 0;
}