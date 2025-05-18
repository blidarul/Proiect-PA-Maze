#include "title.h"
#include <stdio.h>
#include <stdlib.h>

Settings app_settings;

void initialize_menu(Menu *menu, int screen_width, int screen_height)
{
    menu->active = true;

    menu->start_button.rectangle = (Rectangle)
    {
        screen_width/2 - 100,
        screen_height/2 - 100,
        200,
        50
    };

    menu->start_button.text = "START GAME";
    menu->start_button.bg_color = GRAY;
    menu->start_button.text_color = WHITE;

    menu->settings_button.rectangle = (Rectangle)
    {
    screen_width/2 - 100,
    screen_height/2 + 0,  
    200,
    50
    };

    menu->settings_button.text = "SETTINGS";
    menu->settings_button.bg_color = GRAY;
    menu->settings_button.text_color = BLACK;

    menu->exit_button.rectangle = (Rectangle)
    {
        screen_width/2 - 100,
        screen_height/2 + 100,
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
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    Menu_Result result = MENU_NONE;

    // Remove direct color changes from here if DrawEnhancedButton handles hover visuals
    // if (CheckCollisionPointRec(mouse_point, menu->start_button.rectangle))
    //     menu->start_button.bg_color = GRAY; // Or keep this for non-hovered state
    // else
    //     menu->start_button.bg_color = LIGHTGRAY;

    // ... (similar for other buttons) ...

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
                 button->rectangle.x + (button->rectangle.width - txt_w)/2,
                 button->rectangle.y + (button->rectangle.height - 20)/2,
                 20,
                 currentTextColor);
    };

    // Start Button
    bool startHovered = CheckCollisionPointRec(mousePoint, menu->start_button.rectangle);
    DrawEnhancedButton(&menu->start_button, startHovered);

    // Settings Button
    bool settingsHovered = CheckCollisionPointRec(mousePoint, menu->settings_button.rectangle);
    DrawEnhancedButton(&menu->settings_button, settingsHovered);
    
    // Exit Button
    bool exitHovered = CheckCollisionPointRec(mousePoint, menu->exit_button.rectangle);
    DrawEnhancedButton(&menu->exit_button, exitHovered);
}

void unload_menu(Menu *menu)
{
    (void)menu;
}