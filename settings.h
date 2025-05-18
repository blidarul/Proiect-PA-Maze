#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include "raylib.h"

typedef struct 
{
    float volume;          
    float sensitivity;     
    int character_size;         

    Rectangle volume_slider;      
    Rectangle sensitivity_slider;
    Rectangle charsize_slider;
    Rectangle back_button; // It's good practice to store button rects here too

    bool volume_dragging;
    bool sensitivity_dragging;
    bool charsize_dragging;
} Settings;

void initialize_settings(Settings *settings, int screen_width, int screen_height);

bool update_settings(Settings *settings);

void draw_settings(const Settings *settings);

#endif