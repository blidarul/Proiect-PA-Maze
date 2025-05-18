#include "questions.h"
#include <stdio.h>
#include <string.h>
#include "gameState.h"
#include "raylib.h"
#include "game.h"
#include "raygui.h"
void LoadQuestions(Question *questions)
{
    FILE *questionText = fopen("resources/Q_A/Questions.txt", "r");
    FILE *answerText = fopen("resources/Q_A/Answers.txt", "r");
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

void DrawQuestionWindow(Question *questions, int random)
{
    // Define the dimensions of the window
    float windowWidth = 600;
    float windowHeight = 400;
    float windowX = (GetScreenWidth() - windowWidth) / 2.0f; // Center horizontally
    float windowY = (GetScreenHeight() - windowHeight) / 2.0f; // Center vertically

    // Draw the window
    GuiWindowBox((Rectangle){windowX, windowY, windowWidth, windowHeight}, "Question");

    // Define spacing and dimensions for the question and answers
    float padding = 20;
    float questionHeight = 60;
    float answerHeight = 40;
    float buttonWidth = 50;
    float buttonHeight = 30;
    float answerSpacing = 10;

    // Draw the question text
    GuiLabel((Rectangle){windowX + padding, windowY + padding, windowWidth - 2 * padding, questionHeight}, questions[random].questionText);

    // Draw the answers and buttons
    for (int i = 0; i < 4; i++)
    {
        float answerY = windowY + padding + questionHeight + (answerHeight + answerSpacing) * i;

        // Draw the answer text
        GuiLabel((Rectangle){windowX + padding + buttonWidth + 10, answerY, windowWidth - 2 * padding - buttonWidth - 10, answerHeight}, questions[random].answersText[i]);

        // Draw the corresponding button
        char buttonLabel[2] = {'A' + i, '\0'}; // Generate button labels A, B, C, D
        if (GuiButton((Rectangle){windowX + padding, answerY, buttonWidth, buttonHeight}, buttonLabel))
        {
            // Handle button click
            printf("Button %c clicked!\n", 'A' + i);

            // Transition back to gameplay
            isQuestionActive = false;
            SetGameState(GAME_STATE_GAMEPLAY);
        }
    }
}