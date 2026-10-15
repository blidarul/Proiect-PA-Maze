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
    case GAME_STATE_PAUSE:
    case GAME_STATE_QUESTION:
    case GAME_STATE_SETTINGS:
    case GAME_STATE_WIN:
        EnableCursor();
        break;
    default:
        EnableCursor(); 
        break;
    }
}

void InitGameState(void)
{
    currentState = GAME_STATE_TITLE;
    EnableCursor();
}