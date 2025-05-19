#include "title.h"
#include <stdio.h>
#include <stdlib.h>

Settings app_settings;

void initialize_menu(Menu *menu, int screen_width, int screen_height)
{
    menu->active = true;
    float button_y_start = screen_height / 2.0f - 150.0f; // Adjusted for 4 buttons
    float button_spacing = 75.0f; // 50px height + 25px gap

    menu->start_button.rectangle = (Rectangle)
    {
        screen_width/2.0f - 100,
        button_y_start,
        200,
        50
    };
    menu->start_button.text = "START GAME";
    menu->start_button.bg_color = GRAY;
    menu->start_button.text_color = WHITE;

    menu->settings_button.rectangle = (Rectangle)
    {
        screen_width/2.0f - 100,
        button_y_start + button_spacing,
        200,
        50
    };
    menu->settings_button.text = "SETTINGS";
    menu->settings_button.bg_color = GRAY;
    menu->settings_button.text_color = BLACK;

    menu->help_button.rectangle = (Rectangle) // New Help Button
    {
        screen_width/2.0f - 100,
        button_y_start + 2 * button_spacing,
        200,
        50
    };
    menu->help_button.text = "HELP";
    menu->help_button.bg_color = GRAY;
    menu->help_button.text_color = BLACK;

    menu->exit_button.rectangle = (Rectangle)
    {
        screen_width/2.0f - 100,
        button_y_start + 3 * button_spacing,
        200,
        50
    };
    menu->exit_button.text = "EXIT";
    menu->exit_button.bg_color = GRAY;
    menu->exit_button.text_color = BLACK;
}

Menu_Result update_menu(Menu *menu)
{
    Vector2 mouse_point = GetMousePosition();
    int screen_width = GetScreenWidth(); // Keep if needed for settings
    int screen_height = GetScreenHeight(); // Keep if needed for settings
    Menu_Result result = MENU_NONE;

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse_point, menu->start_button.rectangle))
        {
            menu->active = false;
            result = MENU_START;
        }
        else if (CheckCollisionPointRec(mouse_point, menu->settings_button.rectangle))
        {
            initialize_settings(&app_settings, screen_width, screen_height);
            result = MENU_SETTINGS;
        }
        else if (CheckCollisionPointRec(mouse_point, menu->help_button.rectangle)) // Handle Help Button
        {
            result = MENU_SHOW_HELP;
        }
        else if (CheckCollisionPointRec(mouse_point, menu->exit_button.rectangle))
        {
            result = MENU_EXIT;
        }
    }
    return result;
}

void draw_menu(const Menu *menu)
{
    // --- Enhanced Title Text ---
    const char *header = "MAIN MENU";
    int headerFontSize = 40; // Increased font size
    Color headerColor = DARKBLUE; // Or another color like GOLD, MAROON

    const char *faculty = "Polytehnic University of Bucharest";
    const char *proffesor = "Proffersors: Caramihai Dan-Mihail, Chis Daniel-Ioan";
    const char *contributors = "Contributors: Blidaru Andrei-Eduard, Chiriac Mihai-Ciprian, Solca Andrei-Rares, Stan Dragos-Ioan, Matei Radu-Stefan, Delea Robert-Stelian";

    int header_w = MeasureText(header, headerFontSize);
    DrawText(header,
             GetScreenWidth()/2 - header_w/2,
             80, // Adjusted Y position
             headerFontSize,
             headerColor);

    // --- Enhanced Button Drawing ---
    Vector2 mousePoint = GetMousePosition(); // Get mouse position once for hover checks

    // Helper function to draw a single button with enhancements
    auto void DrawEnhancedButton(const Button *button, bool isHovered)
    {
        Color currentBgColor = button->bg_color;
        Color currentTextColor = button->text_color;
        Color borderColor = BLACK;
        float borderThickness = 2.0f;

        if (isHovered)
        {
            currentBgColor = DARKGRAY; // Darker hover
            currentTextColor = WHITE;
            borderColor = WHITE; // Brighter border on hover
        }

        // Draw border
        DrawRectangleLinesEx(button->rectangle, borderThickness, borderColor);
        
        // Draw background
        DrawRectangleRec(button->rectangle, currentBgColor);
        
        // Draw text
        int txt_w = MeasureText(button->text, 20);
        DrawText(button->text,
                 button->rectangle.x + (button->rectangle.width - txt_w)/2.0f,
                 button->rectangle.y + (button->rectangle.height - 20)/2.0f,
                 20,
                 currentTextColor);
    };

    // Start Button
    bool startHovered = CheckCollisionPointRec(mousePoint, menu->start_button.rectangle);
    DrawEnhancedButton(&menu->start_button, startHovered);

    // Settings Button
    bool settingsHovered = CheckCollisionPointRec(mousePoint, menu->settings_button.rectangle);
    DrawEnhancedButton(&menu->settings_button, settingsHovered);

    // Help Button
    bool helpHovered = CheckCollisionPointRec(mousePoint, menu->help_button.rectangle);
    DrawEnhancedButton(&menu->help_button, helpHovered);
    
    // Exit Button
    bool exitHovered = CheckCollisionPointRec(mousePoint, menu->exit_button.rectangle);
    DrawEnhancedButton(&menu->exit_button, exitHovered);

    //draw credentials
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int current_y_offset = screen_height - 100; // Start drawing from the bottom up

    int faculty_w = MeasureText(faculty, 20);
    DrawText(faculty,
             screen_width / 2 - faculty_w / 2,
             current_y_offset,
             20,
             BLACK);
    current_y_offset += 25; // Move up for the next line

    int proffesor_w = MeasureText(proffesor, 20);
    DrawText(proffesor,
             screen_width / 2 - proffesor_w / 2,
             current_y_offset,
             20,
             BLACK);
    current_y_offset += 25;

    int contributors_w = MeasureText(contributors, 15); // Smaller font for potentially long text
    DrawText(contributors,
             screen_width / 2 - contributors_w / 2,
             current_y_offset,
             15, // Smaller font size
             BLACK);
}

// New function to draw the help screen content
void draw_help_screen(void) {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    const char* title_text = "Controls & Tutorial";
    int title_font_size = 30;
    int title_text_width = MeasureText(title_text, title_font_size);
    DrawText(title_text, screen_width / 2 - title_text_width / 2, screen_height / 4 - 60, title_font_size, DARKGRAY);

    int text_font_size = 20;
    int line_spacing = 30;
    int start_y = screen_height / 4;

    const char* line1 = "W, A, S, D to move";
    const char* line2 = "SHIFT to sprint";
    const char* line3 = "Answer questions to reveal some of the maze";
    const char* line4 = "Reach the bottom-right corner to win (green on the minimap)";
    const char* line5 = "Change interesting options in the SETTINGS tab!"; // New line
    const char* return_msg = "Press ESC to return";

    DrawText(line1, screen_width / 2 - MeasureText(line1, text_font_size) / 2, start_y, text_font_size, BLACK);
    start_y += line_spacing;
    DrawText(line2, screen_width / 2 - MeasureText(line2, text_font_size) / 2, start_y, text_font_size, BLACK);
    start_y += line_spacing;
    DrawText(line3, screen_width / 2 - MeasureText(line3, text_font_size) / 2, start_y, text_font_size, BLACK);
    start_y += line_spacing;
    DrawText(line4, screen_width / 2 - MeasureText(line4, text_font_size) / 2, start_y, text_font_size, BLACK);
    start_y += line_spacing; // Add spacing for the new line
    DrawText(line5, screen_width / 2 - MeasureText(line5, text_font_size) / 2, start_y, text_font_size, BLACK); // Draw the new line

    start_y += line_spacing * 2; 
    DrawText(return_msg, screen_width / 2 - MeasureText(return_msg, text_font_size) / 2, start_y, text_font_size, DARKGRAY);
}

void unload_menu(Menu *menu)
{
    (void)menu;
}