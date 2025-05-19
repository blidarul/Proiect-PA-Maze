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

void DrawQuestionWindow(Question *questions, int random, Sound correctAnswerSound, Sound incorrectAnswerSound, GameResources *resources)
{
    // Define the dimensions of the window
    float windowWidth = 600;
    float windowHeight = 400;
    float windowX = (GetScreenWidth() - windowWidth) / 2.0f; 
    float windowY = (GetScreenHeight() - windowHeight) / 2.0f;

    GuiWindowBox((Rectangle){windowX, windowY, windowWidth, windowHeight}, "Question");

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

    float questionTextY = windowY + titleBarHeight + padding;

    // --- TEXT DRAWING WITH MANUAL WORD WRAP ---
    Font font = GuiGetFont();
    float fontSize = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
    float characterSpacing = (float)GuiGetStyle(DEFAULT, TEXT_SPACING); 
    Color textColor = GetColor(GuiGetStyle(LABEL, TEXT_COLOR_NORMAL));
    
    float textBlockX = windowX + padding;
    float textBlockY = questionTextY;
    float textBlockWidth = windowWidth - 2 * padding;
    float textBlockHeight = questionHeight; 

    float currentY = textBlockY;
    float lineHeight = fontSize + lineSpacing; // fontSize + a small gap for line spacing

    const char *text = questions[random].questionText;
    char currentLine[1024] = {0}; 
    int currentLineLength = 0;
    const char *ptr = text;
    while (*ptr) {
        char word[256] = {0}; 
        int wordLength = 0;
        bool isExplicitNewline = false;
        while (*ptr && *ptr != ' ' && *ptr != '\n') {
            if (wordLength < 255) word[wordLength++] = *ptr;
            ptr++;
        }
        word[wordLength] = '\0'; 
        if (*ptr == '\n') { isExplicitNewline = true; ptr++; } 
        else if (*ptr == ' ') { ptr++; }
        if (wordLength > 0) {
            char testLine[1024];
            strcpy(testLine, currentLine);
            if (currentLineLength > 0) strcat(testLine, " ");
            strcat(testLine, word);
            Vector2 testLineSize = MeasureTextEx(font, testLine, fontSize, characterSpacing);
            if (currentLineLength > 0 && testLineSize.x > textBlockWidth) {
                if (currentY + fontSize <= textBlockY + textBlockHeight) {
                    DrawTextEx(font, currentLine, (Vector2){textBlockX, currentY}, fontSize, characterSpacing, textColor);
                }
                currentY += lineHeight;
                currentLine[0] = '\0'; 
                currentLineLength = 0;
                if (currentY + fontSize > textBlockY + textBlockHeight) break; 
                strcpy(currentLine, word);
                currentLineLength = wordLength;
            } else {
                strcpy(currentLine, testLine);
                currentLineLength = strlen(currentLine);
            }
        }
        if (isExplicitNewline) {
            if (currentY + fontSize <= textBlockY + textBlockHeight) {
                DrawTextEx(font, currentLine, (Vector2){textBlockX, currentY}, fontSize, characterSpacing, textColor);
            }
            currentY += lineHeight;
            currentLine[0] = '\0'; 
            currentLineLength = 0;
            if (currentY + fontSize > textBlockY + textBlockHeight) break; 
        }
    }
    if (currentLineLength > 0 && (currentY + fontSize <= textBlockY + textBlockHeight)) {
        DrawTextEx(font, currentLine, (Vector2){textBlockX, currentY}, fontSize, characterSpacing, textColor);
    }
    // --- END OF TEXT DRAWING ---

    GuiSetStyle(DEFAULT, TEXT_SIZE, originalTextSize);

    // Draw the answers and buttons
    float firstAnswerY = questionTextY + questionHeight + answerSpacing; 

    for (int i = 0; i < 4; i++)
    {
        float currentAnswerY = firstAnswerY + (answerHeight + answerSpacing) * i;
        GuiLabel((Rectangle){windowX + padding + buttonWidth + 10, currentAnswerY, 
                 windowWidth - 2 * padding - buttonWidth - 10, answerHeight}, 
                 questions[random].answersText[i]);
        
        char buttonLabel[2] = {'a' + i, '\0'}; 
        if (GuiButton((Rectangle){windowX + padding, currentAnswerY, buttonWidth, buttonHeight}, buttonLabel))
        {
            char chosenAnswerChar = 'a' + i;
            char correctAnswerChar = questions[random].answers[0];

            if (chosenAnswerChar == correctAnswerChar)
            {
                PlaySound(correctAnswerSound);
                
                // Start revealing from the bottom of the minimap
                static int lastRevealedRow = 0;
                if (lastRevealedRow == 0) 
                {
                    lastRevealedRow = resources->minimap.height - 1; // Start from bottom
                }
                
                // Reveal 3 new rows going upward
                int rowsToReveal = 3;
                
                // Calculate win position (bottom-right corner of the maze)
                // The maze dimensions are typically based on the cubicimage dimensions
                int mazeWidth = resources->cubicimage.width;
                int mazeHeight = resources->cubicimage.height;
                int exitX = mazeWidth - 1;  // Right-most cell
                int exitY = mazeHeight - 1; // Bottom-most cell
                
                // Reveal several new rows (going up from the bottom)
                for(int y = lastRevealedRow; y > lastRevealedRow - rowsToReveal && y >= 0; y--)
                {
                    for(int x = 0; x < resources->minimap.width; x++)
                    {
                        // Get the color from the cubicmap (which contains the full maze)
                        Color pixelColor = GetImageColor(resources->cubicimage, x, y);
                        
                        // If the pixel is a path (black in the cubicmap)
                        if (pixelColor.r == 0)
                        {
                            // Check if this is the win position
                            if (x == exitX && y == exitY)
                            {
                                // Mark the win position as GREEN
                                ImageDrawPixel(&resources->minimap, x, y, GREEN);
                            }
                            else
                            {
                                // Reveal this pixel on the minimap as WHITE
                                ImageDrawPixel(&resources->minimap, x, y, WHITE);
                            }
                        }
                    }
                }
                
                // Update the last revealed row (moving upward)
                lastRevealedRow -= rowsToReveal;
                
                // Update the minimap texture
                UpdateMinimapTexture(resources);
            }
            else
            {
                PlaySound(incorrectAnswerSound);
            }
            
            SetGameState(GAME_STATE_GAMEPLAY);
        }
    }
}