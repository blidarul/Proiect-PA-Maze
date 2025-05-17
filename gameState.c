#include "gameState.h"
#include "raylib.h"

static GameState currentState = GAME_STATE_TITLE;

GameState GetGameState(void)
{
    return currentState;
}

void SetGameState(GameState state)
{
    currentState = state;
    switch (state)
    {
    case GAME_STATE_GAMEPLAY:
        DisableCursor();
        break;
    case GAME_STATE_TITLE:
        EnableCursor();
        break;
    case GAME_STATE_QUESTION:
        EnableCursor();
        break;
    case GAME_STATE_PAUSE:
        EnableCursor();
        break;
    default:
        break;
    }
}

void InitGameState(void)
{
    currentState = GAME_STATE_TITLE;
}