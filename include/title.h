#ifndef TITLE_H
#define TITLE_H

#include <stdbool.h>
#include "raylib.h"
#include "settings.h" // Ensure settings.h is included

extern Settings app_settings; // Declare app_settings for external access

typedef enum {
    MENU_NONE,
    MENU_START,
    MENU_SETTINGS,
    MENU_EXIT,
    MENU_SHOW_HELP
} Menu_Result;

typedef struct
{
    Rectangle rectangle;
    const char *text;
    Color bg_color;
    Color text_color;
}Button;

typedef struct Menu
{
    bool active;
    Button start_button;
    Button settings_button;
    Button help_button;    // Add this
    Button exit_button;
}Menu;

void initialize_menu(Menu *menu, int screen_width, int screen_height);
Menu_Result update_menu(Menu *menu);
void draw_menu(const Menu *menu);
void unload_menu(Menu *menu);

// Add declaration for the new draw_help_screen function
void draw_help_screen(void);

#endif