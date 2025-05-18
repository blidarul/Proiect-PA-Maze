#ifndef PAUSE_H
#define PAUSE_H

#include<stdbool.h>
#include "raylib.h"

typedef enum
{
    MENU_ACTION_NONE = 0,
    MENU_ACTION_RESUME,
    MENU_ACTION_RESTART,
    MENU_ACTION_EXIT
} Menu_action;

typedef struct
{
    Rectangle rect;
    const char *label;
    Color bg_color;
    Color text_color;
    Color hover_color;
} PauseButton;

typedef struct
{
    Rectangle bar;
    Rectangle handle;
    float volume_value;
    bool dragging;
} PauseSlider;

typedef struct
{
    bool active;
    PauseButton resume_button;
    PauseButton restart_button;
    PauseButton exit_button;
    PauseSlider volume_slider;
    Menu_action action;
} Pause_menu;

Menu_action InitializePauseControls(Pause_menu *menu, int screen_width, int screen_height);
Menu_action UpdatePauseControls(Pause_menu *menu);
void DrawPauseControls(const Pause_menu *menu);
void UnloadPauseControls(Pause_menu *menu);
// ...

#endif
