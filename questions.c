#include "questions.h"
#include <stdio.h>
#include <string.h>
void LoadQuestions(Question *questions)
{
    FILE *questionText = fopen("resources/Questions.txt", "r");
    FILE *answerText = fopen("resources/Answers.txt", "r");
    char buffer[1024];
    int i = 0, j, index;
    while (fgets(buffer, QUESTION_SIZE, questionText))
    {
        strcpy(questions[i].questionText, buffer);
        i++;
    }
    i = 0;
    while (fgets(buffer, QUESTION_SIZE, answerText))
    {
        index = 0;
        for (j = 0; j < strlen(buffer); j++)
        {
            if (buffer[j] >= 'a' && buffer[j] <= 'z')
            {
                questions[i].answers[index] = buffer[j];
                index++;
            }
        }
        questions[i].answers[index] = '\0';
        // reading all the possible answers into the question
        fgets(buffer, QUESTION_SIZE, answerText);
        strcpy(questions[i].answersText[0], buffer);
        fgets(buffer, QUESTION_SIZE, answerText);
        strcpy(questions[i].answersText[1], buffer);
        fgets(buffer, QUESTION_SIZE, answerText);
        strcpy(questions[i].answersText[2], buffer);
        fgets(buffer, QUESTION_SIZE, answerText);
        strcpy(questions[i].answersText[3], buffer);
        i++;
    }
}

