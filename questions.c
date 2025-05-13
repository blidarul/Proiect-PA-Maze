#include "questions.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Question* loadQuestions(char *questionsPath, char *answersPath)
{
    FILE *questionsFile = fopen(questionsPath, "r");
    if(questionsFile == NULL)
    {
        return NULL;
    }

    FILE *answersFile = fopen(answersPath, "r");
    if(answersFile == NULL)
    {
        fclose(questionsFile);
        return NULL;
    }

    int questionCount = 0;
    char buffer[1024];
    
    while(fgets(buffer, sizeof(buffer), questionsFile))
    {
        if(strlen(buffer) > 1)
            questionCount++;
    }
    
    rewind(questionsFile);
    
    Question *questions = (Question*)malloc(questionCount * sizeof(Question));
    if(questions == NULL)
    {
        fclose(questionsFile);
        fclose(answersFile);
        return NULL;
    }
    
    for(int i = 0; i < questionCount; i++)
    {
        if(!fgets(buffer, sizeof(buffer), questionsFile))
            break;
            
        buffer[strcspn(buffer, "\n")] = 0;
        
        char* dot = strchr(buffer, '.');
        if(dot)
        {
            strcpy(questions[i].text, dot + 1);

            char *start = questions[i].text;
            while(isspace((unsigned char)*start)) start++;
            
            if(start != questions[i].text)
                memmove(questions[i].text, start, strlen(start) + 1);
        }
        else
        {
            strcpy(questions[i].text, buffer);
        }
        
        char answerLine[10];
        if(!fgets(answerLine, sizeof(answerLine), answersFile))
            break;
        
        char correctAnswer = answerLine[2];
        questions[i].correctAnswer = correctAnswer - 'A';
        
        for(int j = 0; j < 4; j++)
        {
            char answerBuffer[1024];
            if(!fgets(answerBuffer, sizeof(answerBuffer), answersFile))
                break;
            
            answerBuffer[strcspn(answerBuffer, "\n")] = 0;
            
            char* start = answerBuffer + 3;
            strcpy(questions[i].answers[j], start);
        }
    }
    
    fclose(questionsFile);
    fclose(answersFile);
    
    return questions;
}