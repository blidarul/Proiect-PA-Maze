#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include "raylib.h"

// Define min/max for the player height slider


typedef struct 
{
    float volume;          
    float sensitivity;     
    float player_height; // New field for player height

    Rectangle volume_slider;      
    Rectangle sensitivity_slider;
    Rectangle player_height_slider; // New field
    Rectangle back_button;

    bool volume_dragging;
    bool sensitivity_dragging;
    bool player_height_dragging; // New field
} Settings;

void initialize_settings(Settings *settings, int screen_width, int screen_height);
bool update_settings(Settings *settings);
void draw_settings(const Settings *settings);

#endif