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

Menu_action InitializePauseControls(Pause_menu *menu, int screen_width, int screen_height)
{
    menu->active = true;
    menu->action = MENU_ACTION_NONE;

    int cx = screen_width / 2;
    int cy = screen_height / 2;

    InitPauseButton(&menu->resume_button,  cx - 100, cy - 100, 200, 40, "RESUME", DARKGRAY, GRAY, WHITE);
    InitPauseButton(&menu->settings_button, cx - 100, cy - 40,  200, 40, "SETTINGS", DARKGRAY, GRAY, WHITE);
    InitPauseButton(&menu->exit_button,    cx - 100, cy + 20,   200, 40, "EXIT", DARKGRAY, GRAY, WHITE);
    
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

    return menu->action;
}

void DrawPauseControls(const Pause_menu *menu)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    DrawSinglePauseButton(&menu->resume_button);
    DrawSinglePauseButton(&menu->settings_button);
    DrawSinglePauseButton(&menu->exit_button);

    const char* pausedText = "PAUSED";
    int pausedTextWidth = MeasureText(pausedText, 40);
    DrawText(pausedText, GetScreenWidth()/2 - pausedTextWidth/2, GetScreenHeight()/2 - 180, 40, WHITE);
}

void UnloadPauseControls(Pause_menu *menu)
{
    (void)menu;
}