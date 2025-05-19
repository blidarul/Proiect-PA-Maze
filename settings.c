#include "settings.h"
#include "raygui.h"
#include "sound.h"
#include "config.h"

static float clamp(float value, float min, float max) 
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void initialize_settings(Settings *settings, int screen_width, int screen_height)
{
    settings->volume = 0.5f;
    settings->sensitivity = 50.0f;
    settings->player_height = DEFAULT_PLAYER_HEIGHT_SETTING;

    settings->volume_dragging = false;
    settings->sensitivity_dragging = false;
    settings->player_height_dragging = false;

    int slider_width = 200;
    int slider_height = 20;
    int start_x = screen_width / 2 - slider_width / 2;
    int start_y = 100;
    int y_spacing = 80;

    settings->volume_slider = (Rectangle){start_x, start_y, slider_width, slider_height};
    settings->sensitivity_slider = (Rectangle){start_x, start_y + y_spacing, slider_width, slider_height};
    settings->player_height_slider = (Rectangle){start_x, start_y + 2 * y_spacing, slider_width, slider_height};

    settings->back_button = (Rectangle){ start_x, start_y + 3 * y_spacing + 20, slider_width, 40 };
}

bool update_settings(Settings *settings)
{
    Vector2 mouse_pos = GetMousePosition();

    float get_slider_value_normalized(Rectangle slider_rect) 
    {
        float val = (mouse_pos.x - slider_rect.x) / slider_rect.width;
        return clamp(val, 0.0f, 1.0f);
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
    {
        if (CheckCollisionPointRec(mouse_pos, settings->volume_slider)) settings->volume_dragging = true;
        else if (CheckCollisionPointRec(mouse_pos, settings->sensitivity_slider)) settings->sensitivity_dragging = true;
        else if (CheckCollisionPointRec(mouse_pos, settings->player_height_slider)) settings->player_height_dragging = true;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) 
    {
        settings->volume_dragging = false;
        settings->sensitivity_dragging = false;
        settings->player_height_dragging = false;
    }

    if (settings->volume_dragging) 
    {
        float val = get_slider_value_normalized(settings->volume_slider);
        settings->volume = val;  
        SetMasterVolume(settings->volume); 
    }
    if (settings->sensitivity_dragging) 
    {
        float val = get_slider_value_normalized(settings->sensitivity_slider);
        settings->sensitivity = 10.0f + val * (100.0f - 10.0f);
    }
    if (settings->player_height_dragging) 
    {
        float val_norm = get_slider_value_normalized(settings->player_height_slider);
        settings->player_height = MIN_PLAYER_HEIGHT_SETTING + val_norm * (MAX_PLAYER_HEIGHT_SETTING - MIN_PLAYER_HEIGHT_SETTING);
    }

    bool backButtonPressed = false;
    if (GuiButton(settings->back_button, "Back")) 
    {
        backButtonPressed = true;
    }

    return backButtonPressed;
}

void draw_settings(const Settings *settings)
{
    const int fontSize = 20;
    int screen_width = GetScreenWidth();

    DrawText("Settings", screen_width / 2 - MeasureText("Settings", fontSize + 10) / 2, 30, fontSize + 10, DARKBLUE);

    DrawText("Volume", settings->volume_slider.x, settings->volume_slider.y - 25, fontSize, BLACK);
    DrawRectangleRec(settings->volume_slider, LIGHTGRAY);
    DrawCircle(settings->volume_slider.x + (int)(settings->volume * settings->volume_slider.width), 
               settings->volume_slider.y + settings->volume_slider.height / 2, 10, DARKBLUE);
    DrawText(TextFormat("%.0f%%", settings->volume * 100), settings->volume_slider.x + settings->volume_slider.width + 20, settings->volume_slider.y, fontSize, BLACK);

    DrawText("Sensitivity", settings->sensitivity_slider.x, settings->sensitivity_slider.y - 25, fontSize, BLACK);
    DrawRectangleRec(settings->sensitivity_slider, LIGHTGRAY);
    float sensitivity_norm = (settings->sensitivity - 10.0f) / (100.0f - 10.0f);
    DrawCircle(settings->sensitivity_slider.x + (int)(sensitivity_norm * settings->sensitivity_slider.width), 
               settings->sensitivity_slider.y + settings->sensitivity_slider.height / 2, 10, DARKBLUE);
    DrawText(TextFormat("%.0f", settings->sensitivity), settings->sensitivity_slider.x + settings->sensitivity_slider.width + 20, settings->sensitivity_slider.y, fontSize, BLACK);
    
    DrawText("Player Height", settings->player_height_slider.x, settings->player_height_slider.y - 25, fontSize, BLACK);
    DrawRectangleRec(settings->player_height_slider, LIGHTGRAY);
    float player_height_norm = (settings->player_height - MIN_PLAYER_HEIGHT_SETTING) / (MAX_PLAYER_HEIGHT_SETTING - MIN_PLAYER_HEIGHT_SETTING);
    DrawCircle(settings->player_height_slider.x + (int)(player_height_norm * settings->player_height_slider.width), 
               settings->player_height_slider.y + settings->player_height_slider.height / 2, 10, DARKBLUE);
    DrawText(TextFormat("%.2f m", settings->player_height), settings->player_height_slider.x + settings->player_height_slider.width + 20, settings->player_height_slider.y, fontSize, BLACK);

    DrawRectangleRec(settings->back_button, LIGHTGRAY);
    DrawText("Back", settings->back_button.x + settings->back_button.width / 2 - MeasureText("Back", fontSize) / 2, 
             settings->back_button.y + settings->back_button.height / 2 - fontSize / 2, fontSize, BLACK);
}
