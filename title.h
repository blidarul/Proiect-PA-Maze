#ifndef TITLE_H
#define TITLE_H

#include<stdbool.h>
#include "raylib.h"
#include "settings.h"

typedef enum {
    MENU_NONE,
    MENU_START,
    MENU_SETTINGS,
    MENU_EXIT
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
    Button exit_button;
}Menu;

void initialize_menu(Menu *menu, int screen_width, int screen_height);

void update_menu(Menu *menu);

void draw_menu(const Menu *menu);

void unload_menu(Menu *menu);

#endif