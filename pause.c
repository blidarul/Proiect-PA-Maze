#include "pause.h"
#include <stdio.h>
#include "raylib.h"
#include "sound.h"

static void InitPauseButton(PauseButton *button, int x, int y, int width, int height, const char *label, Color bg, Color hover, Color text)
{
    button->rect = (Rectangle){x, y, width, height};
    button->label = label;
    button->bg_color = bg;
    button->hover_color = hover;
    button->text_color = text;
}

static void DrawSinglePauseButton(const PauseButton *button)
{
    Vector2 mouse = GetMousePosition();
    Color col;
    bool hover = CheckCollisionPointRec(mouse, button->rect);

    if (hover)
    {
        col = button->hover_color;
    }
    else
    {
        col = button->bg_color;
    }
    DrawRectangleRec(button->rect, col);

    int tw = MeasureText(button->label, 20);
    DrawText(button->label,
             button->rect.x + (button->rect.width - tw)/2,
             button->rect.y + (button->rect.height - 20)/2,
             20,
             button->text_color);
}

static void InitPauseSlider(PauseSlider *slider, int x, int y, int length, int thickness)
{
    slider->bar = (Rectangle){x, y, length, thickness};
    slider->handle = (Rectangle){x + length * slider->volume_value - thickness/2, y - thickness/2 + thickness/2, thickness, thickness * 2};
    slider->dragging = false;
}

static void DrawSinglePauseSlider(const PauseSlider *slider)
{
    DrawRectangleRec(slider->bar, LIGHTGRAY);
    DrawRectangleRec(slider->handle, DARKGRAY);
}

Menu_action InitializePauseControls(Pause_menu *menu, int screen_width, int screen_height)
{
    menu->active = true;
    menu->action = MENU_ACTION_NONE;

    int cx = screen_width / 2;
    int cy = screen_height / 2;

    InitPauseButton(&menu->resume_button,  cx - 100, cy - 120, 200, 40, "RESUME", DARKGRAY, GRAY, WHITE);
    InitPauseButton(&menu->settings_button, cx - 100, cy - 60,  200, 40, "SETTINGS", DARKGRAY, GRAY, WHITE);
    InitPauseButton(&menu->exit_button,    cx - 100, cy + 0,   200, 40, "EXIT", DARKGRAY, GRAY, WHITE);
    
    menu->volume_slider.volume_value = GetCurrentMusicVolume();
    InitPauseSlider(&menu->volume_slider, cx - 100, cy + 80, 200, 10);
    
    return menu->action;
}

Menu_action UpdatePauseControls(Pause_menu *menu)
{
    menu->action = MENU_ACTION_NONE;
    Vector2 mouse = GetMousePosition();

    // Resume button
    if (CheckCollisionPointRec(mouse, menu->resume_button.rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        menu->action = MENU_ACTION_RESUME;
    }

    // Settings button
    if (CheckCollisionPointRec(mouse, menu->settings_button.rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        menu->action = MENU_ACTION_SETTINGS;
    }
    
    // Exit button
    if (CheckCollisionPointRec(mouse, menu->exit_button.rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        menu->action = MENU_ACTION_EXIT;
    }

    // Slider logic
    PauseSlider *sld = &menu->volume_slider;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, sld->handle))
    {
        sld->dragging = true;
    }

    if (sld->dragging)
    {
        float x = mouse.x;
        if (x < sld->bar.x) x = sld->bar.x;
        if (x > sld->bar.x + sld->bar.width) x = sld->bar.x + sld->bar.width;
        
        sld->handle.x = x - sld->handle.width/2;
        sld->volume_value = (x - sld->bar.x) / sld->bar.width;
        
        SetCurrentMusicVolume(sld->volume_value);
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) sld->dragging = false;

    return menu->action;
}

void DrawPauseControls(const Pause_menu *menu)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    DrawSinglePauseButton(&menu->resume_button);
    DrawSinglePauseButton(&menu->settings_button);
    DrawSinglePauseButton(&menu->exit_button);
    
    DrawSinglePauseSlider(&menu->volume_slider);

    const char* pausedText = "PAUSED";
    int pausedTextWidth = MeasureText(pausedText, 40);
    DrawText(pausedText, GetScreenWidth()/2 - pausedTextWidth/2, GetScreenHeight()/2 - 200, 40, WHITE);
}

void UnloadPauseControls(Pause_menu *menu)
{
    (void)menu;
}