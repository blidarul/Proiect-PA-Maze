#ifndef PAUSE_H
#define PAUSE_H

#include<stdbool.h>
#include "raylib.h"

typedef enum 
{
    MENU_NONE = 0,
    MENU_RESUME,
    MENU_RESTART,
    MENU_EXIT
}Menu_action;

typedef struct 
{
    Rectangle rect;
    const char *label;
    Color bg_color;
    Color text_color;
    Color hover_color;
}Button;

typedef struct
{
    Rectangle bar;
    Rectangle handle;
    float volume_value;
    bool dragging;
}Slider;

typedef struct
{
    bool active;
    Button resume_button;
    Button restart_button;
    Button exit_button;
    Slider volume_slider;
    Menu_action action;
}Pause_menu;

Menu_action initialize_menu(Pause_menu *menu, int screen_width, int screen_height);

Menu_action update_menu(Pause_menu *menu);

void draw_menu(Pause_menu *menu); 

void unload_menu(Pause_menu *menu);


#endif
