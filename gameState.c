#include "gameState.h"

static GameState currentState = GAME_STATE_TITLE;

GameState GetGameState(void)
{
    return currentState;
}

void SetGameState(GameState state)
{
    currentState = state;
}

void InitGameState(void)
{
    currentState = GAME_STATE_TITLE;
}