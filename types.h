#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define QUESTION_SIZE 200

// Forward declarations of structures
typedef struct GameResources GameResources;
typedef struct Question Question;

// Define Question structure
struct Question {
    char questionText[QUESTION_SIZE];
    char answers[3];
    char answersText[4][QUESTION_SIZE];
};

#endif // TYPES_H