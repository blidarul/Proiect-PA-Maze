#ifndef QUESTIONS_H
#define QUESTIONS_H
#define QUESTION_SIZE 200
#include "raylib.h"

typedef struct {
    char questionText[QUESTION_SIZE];
    char answers[3];
    char answersText[4][QUESTION_SIZE];
} Question;

void LoadQuestions(Question *questions);
void DrawQuestionWindow(Question *questions, int random, Sound correctAnswerSound, Sound incorrectAnswerSound);
#endif