#include "title.h"
#include <stdio.h>
#include <stdlib.h>


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

Settings *settings;

void update_menu(Menu *menu)
{
    Vector2 mouse_point = GetMousePosition();
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    if (CheckCollisionPointRec(mouse_point, menu->start_button.rectangle))
        menu->start_button.bg_color = GRAY;
    else
        menu->start_button.bg_color = LIGHTGRAY;

    if (CheckCollisionPointRec(mouse_point, menu->settings_button.rectangle))
        menu->settings_button.bg_color = GRAY;
    else
        menu->settings_button.bg_color = LIGHTGRAY;

    if (CheckCollisionPointRec(mouse_point, menu->exit_button.rectangle))
        menu->exit_button.bg_color = GRAY;
    else
        menu->exit_button.bg_color = LIGHTGRAY;

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse_point, menu->start_button.rectangle))
        {
            menu->active = false;  
        }

        else if (CheckCollisionPointRec(mouse_point, menu->settings_button.rectangle))
        {
            initialize_settings(settings,screen_width,screen_height);
        }

        else if (CheckCollisionPointRec(mouse_point, menu->exit_button.rectangle))
        {
            CloseWindow();
            exit(0);
        }
    }
}

void draw_menu(const Menu *menu)
{
    const char *header = "MAIN MENU";

    int header_w = MeasureText(header, 20);
    DrawText(header,
             GetScreenWidth()/2 - header_w/2,
             100,
             20,
             DARKBLUE);

    
    DrawRectangleRec(menu->start_button.rectangle, menu->start_button.bg_color);
    int txt_w = MeasureText(menu->start_button.text, 20);
    DrawText(menu->start_button.text,
             menu->start_button.rectangle.x + (menu->start_button.rectangle.width - txt_w)/2,
             menu->start_button.rectangle.y + (menu->start_button.rectangle.height - 20)/2,
             20,
             menu->start_button.text_color);

    DrawRectangleRec(menu->settings_button.rectangle, menu->settings_button.bg_color);
    txt_w = MeasureText(menu->settings_button.text, 20);
    DrawText(menu->settings_button.text,
         menu->settings_button.rectangle.x + (menu->settings_button.rectangle.width - txt_w)/2,
         menu->settings_button.rectangle.y + (menu->settings_button.rectangle.height - 20)/2,
         20,
         menu->settings_button.text_color);

    
    DrawRectangleRec(menu->exit_button.rectangle, menu->exit_button.bg_color);
    txt_w = MeasureText(menu->exit_button.text, 20);
    DrawText(menu->exit_button.text,
             menu->exit_button.rectangle.x + (menu->exit_button.rectangle.width - txt_w)/2,
             menu->exit_button.rectangle.y + (menu->exit_button.rectangle.height - 20)/2,
             20,
             menu->exit_button.text_color);
}

void unload_menu(Menu *menu)
{
    (void)menu;
}