#ifndef GAME_STATE_H
#define GAME_STATE_H

typedef enum {
    GAME_STATE_TITLE,
    GAME_STATE_GAMEPLAY,
    GAME_STATE_PAUSE,
    GAME_STATE_SETTINGS,
    GAME_STATE_QUESTION,
    GAME_STATE_WIN // New win state
} GameState;

GameState GetGameState(void);
void SetGameState(GameState state);
void InitGameState(void);

#endif // GAME_STATE_H