#ifndef QUESTIONS_H
#define QUESTIONS_H

#include <stdio.h>

typedef struct Question
{
    char text[1024];
    char answers[4][1024];
    int correctAnswer;
} Question;

Question* loadQuestions(char *questionsPath, char *answersPath);
void pop_up_Question(Question *questions, unsigned int question_number);
void unloadQuestions(Question *questions);



#endif