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
    float titleBarHeight = 24.0f;
    float questionHeight = 80;
    float answerHeight = 40;
    float buttonWidth = 50;
    float buttonHeight = 30;
    float answerSpacing = 10;
    float lineSpacing = 3.0f;
    int questionTextSize = 20;

    // --- Make Question Text Bigger ---
    int originalTextSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    GuiSetStyle(DEFAULT, TEXT_SIZE, questionTextSize);

    // Calculate Y position for the question text, accounting for title bar and padding
    float questionTextY = windowY + titleBarHeight + padding;

    // --- TEXT DRAWING WITH MANUAL WORD WRAP ---
    Font font = GuiGetFont();
    float fontSize = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
    float characterSpacing = (float)GuiGetStyle(DEFAULT, TEXT_SPACING); // Spacing between characters
    Color textColor = GetColor(GuiGetStyle(LABEL, TEXT_COLOR_NORMAL));
    
    float textBlockX = windowX + padding;
    float textBlockY = questionTextY;
    float textBlockWidth = windowWidth - 2 * padding;
    float textBlockHeight = questionHeight; // The allocated height for the question text

    float currentY = textBlockY;
    float lineHeight = fontSize + lineSpacing; // fontSize + a small gap for line spacing

    const char *text = questions[random].questionText;
    char currentLine[1024] = {0}; // Buffer for the line being built
    int currentLineLength = 0;

    const char *ptr = text;
    while (*ptr) {
        char word[256] = {0}; // Buffer for the current word
        int wordLength = 0;
        bool isExplicitNewline = false;

        // Extract the next word or segment until a newline
        while (*ptr && *ptr != ' ' && *ptr != '\n')
        {
            if (wordLength < 255)
            {
                word[wordLength++] = *ptr;
            }
            ptr++;
        }
        word[wordLength] = '\0'; // Null-terminate the word

        if (*ptr == '\n')
        {
            isExplicitNewline = true;
            ptr++; // Consume newline
        }
        else if (*ptr == ' ')
        {
            ptr++;
        }
        // If *ptr is '\0', the outer loop will terminate after this iteration

        if (wordLength > 0)
        {
            char testLine[1024];
            strcpy(testLine, currentLine);
            if (currentLineLength > 0)
            {
                strcat(testLine, " ");
            }
            strcat(testLine, word);

            Vector2 testLineSize = MeasureTextEx(font, testLine, fontSize, characterSpacing);

            if (currentLineLength > 0 && testLineSize.x > textBlockWidth)
            {
                // Word doesn't fit, draw the current line
                if (currentY + fontSize <= textBlockY + textBlockHeight)
                { // Check if baseline fits
                    DrawTextEx(font, currentLine, (Vector2){textBlockX, currentY}, fontSize, characterSpacing, textColor);
                }
                currentY += lineHeight;
                currentLine[0] = '\0'; // Reset line
                currentLineLength = 0;

                if (currentY + fontSize > textBlockY + textBlockHeight) break; // Out of vertical space

                // Start new line with the current word
                strcpy(currentLine, word);
                currentLineLength = wordLength;
            }
            else
            {
                // Word fits, or line was empty. Append word to current line.
                strcpy(currentLine, testLine);
                currentLineLength = strlen(currentLine);
            }
        }

        if (isExplicitNewline)
        {
            // Draw the accumulated line before the explicit newline
            if (currentY + fontSize <= textBlockY + textBlockHeight)
            {
                DrawTextEx(font, currentLine, (Vector2){textBlockX, currentY}, fontSize, characterSpacing, textColor);
            }
            currentY += lineHeight;
            currentLine[0] = '\0'; // Reset line
            currentLineLength = 0;
            if (currentY + fontSize > textBlockY + textBlockHeight) break; // Out of vertical space
        }
    }

    // Draw any remaining text in the current line
    if (currentLineLength > 0 && (currentY + fontSize <= textBlockY + textBlockHeight))
    {
        DrawTextEx(font, currentLine, (Vector2){textBlockX, currentY}, fontSize, characterSpacing, textColor);
    }
    // --- END OF TEXT DRAWING ---

    // --- Restore Original Text Size ---
    GuiSetStyle(DEFAULT, TEXT_SIZE, originalTextSize);

    // Draw the answers and buttons
    // Calculate the starting Y position for the first answer
    float firstAnswerY = questionTextY + questionHeight + answerSpacing; // Added answerSpacing after question

    for (int i = 0; i < 4; i++)
    {
        // Calculate Y position for the current answer
        float currentAnswerY = firstAnswerY + (answerHeight + answerSpacing) * i;

        // Draw the answer text
        GuiLabel((Rectangle){windowX + padding + buttonWidth + 10, currentAnswerY, windowWidth - 2 * padding - buttonWidth - 10, answerHeight}, questions[random].answersText[i]);

        // Draw the corresponding button
        char buttonLabel[2] = {'A' + i, '\0'}; // Generate button labels A, B, C, D
        if (GuiButton((Rectangle){windowX + padding, currentAnswerY, buttonWidth, buttonHeight}, buttonLabel))
        {
            // Handle button click
            printf("Button %c clicked!\n", 'A' + i);

            // Transition back to gameplay
            isQuestionActive = false;
            SetGameState(GAME_STATE_GAMEPLAY);
        }
    }
}