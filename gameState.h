#ifndef GAME_STATE_H
#define GAME_STATE_H

typedef enum {
    GAME_STATE_TITLE,
    GAME_STATE_GAMEPLAY,
    GAME_STATE_PAUSE,
    GAME_STATE_QUESTION
} GameState;

// Get current game state
GameState GetGameState(void);

// Set game state
void SetGameState(GameState state);

// Initialize game state
void InitGameState(void);

#endif // GAME_STATE_H