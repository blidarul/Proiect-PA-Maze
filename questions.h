#ifndef QUESTIONS_H
#define QUESTIONS_H

#include "types.h"

void LoadQuestions(Question *questions);
void DrawQuestionWindow(Question *questions, int random, Sound correctAnswerSound, 
                       Sound incorrectAnswerSound, GameResources *resources);

#endif // QUESTIONS_H