#include "settings.h"

void initialize_settings(Settings *settings, int screen_width, int screen_height)
{
    settings->volume = 0.5f;
    settings->sensitivity = 50.0f;
    settings->character_size = 20;

    int slider_width = 300;
    int slider_height = 20;
    int start_x = screen_width / 2 - slider_width / 2;
    int start_y = screen_height / 2 - 80;

    settings->volume_slider = (Rectangle){ start_x, start_y, slider_width, slider_height };
    settings->sensitivity_slider = (Rectangle){ start_x, start_y + 60, slider_width, slider_height };
    settings->charsize_slider = (Rectangle){ start_x, start_y + 120, slider_width, slider_height };

    settings->volume_dragging = false;
    settings->sensitivity_dragging = false;
    settings->charsize_dragging = false;
}

static float clamp(float value, float min, float max) 
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void update_settings(Settings *settings)
{
    Vector2 mouse_pos = GetMousePosition();

    
    float get_slider_value(Rectangle slider) 
    {
        float val = (mouse_pos.x - slider.x) / slider.width;
        return clamp(val, 0.0f, 1.0f);
    }

    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
    {
        if (CheckCollisionPointRec(mouse_pos, settings->volume_slider)) settings->volume_dragging = true;
        else if (CheckCollisionPointRec(mouse_pos, settings->sensitivity_slider)) settings->sensitivity_dragging = true;
        else if (CheckCollisionPointRec(mouse_pos, settings->charsize_slider)) settings->charsize_dragging = true;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) 
    {
        settings->volume_dragging = false;
        settings->sensitivity_dragging = false;
        settings->charsize_dragging = false;
    }

    
    if (settings->volume_dragging) 
    {
        float val = get_slider_value(settings->volume_slider);
        settings->volume = val;  
        SetMasterVolume(settings->volume); 
    }
    if (settings->sensitivity_dragging) 
    {
        float val = get_slider_value(settings->sensitivity_slider);
        settings->sensitivity = 10.0f + val * (100.0f - 10.0f);
    }
    if (settings->charsize_dragging) 
    {
        float val = get_slider_value(settings->charsize_slider);
        settings->character_size = (int)(10 + val * (40 - 10));
    }
}

void draw_settings(const Settings *settings)
{
    const int fontSize = 20;
    int screen_width = GetScreenWidth();

    DrawText("Settings", screen_width / 2 - MeasureText("Settings", fontSize) / 2, 50, fontSize + 10, DARKBLUE);

   
    DrawText("Volume", settings->volume_slider.x, settings->volume_slider.y - 25, fontSize, BLACK);
    DrawText("Sensitivity", settings->sensitivity_slider.x, settings->sensitivity_slider.y - 25, fontSize, BLACK);
    DrawText("Character Size", settings->charsize_slider.x, settings->charsize_slider.y - 25, fontSize, BLACK);

    
    DrawRectangleRec(settings->volume_slider, LIGHTGRAY);
    DrawRectangleRec(settings->sensitivity_slider, LIGHTGRAY);
    DrawRectangleRec(settings->charsize_slider, LIGHTGRAY);

    
    int handle_radius = 10;

    int vol_pos = settings->volume_slider.x + (int)(settings->volume * settings->volume_slider.width);
    DrawCircle(vol_pos, settings->volume_slider.y + settings->volume_slider.height / 2, handle_radius, DARKBLUE);

    int sens_pos = settings->sensitivity_slider.x + (int)(((settings->sensitivity - 10) / 90.0f) * settings->sensitivity_slider.width);
    DrawCircle(sens_pos, settings->sensitivity_slider.y + settings->sensitivity_slider.height / 2, handle_radius, DARKBLUE);

    int char_pos = settings->charsize_slider.x + (int)(((settings->character_size - 10) / 30.0f) * settings->charsize_slider.width);
    DrawCircle(char_pos, settings->charsize_slider.y + settings->charsize_slider.height / 2, handle_radius, DARKBLUE);

    
    DrawText(TextFormat("%.2f", settings->volume), settings->volume_slider.x + settings->volume_slider.width + 20, settings->volume_slider.y, fontSize, BLACK);
    DrawText(TextFormat("%.0f", settings->sensitivity), settings->sensitivity_slider.x + settings->sensitivity_slider.width + 20, settings->sensitivity_slider.y, fontSize, BLACK);
    DrawText(TextFormat("%d", settings->character_size), settings->charsize_slider.x + settings->charsize_slider.width + 20, settings->charsize_slider.y, fontSize, BLACK);
}
