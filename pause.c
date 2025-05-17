#include "pause.h"
#include<stdio.h>

static void init_menu(Button *button, int x, int y, int width, int height, const char *label, Color bg, Color hover, Color text)
{
    button->rect = (Rectangle) {x, y, width, height};
    button->label = label;
    button->bg_color = bg;
    button->hover_color = hover;
    button->text_color = text; 
}

static void draw_button(const Button *button)
{
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, button->rect);

    Color col;
    if (hover)
        col = button->hover_color;
    else
        col = button->bg_color;

    DrawRectangleRec(button->rect, col);

    int tw = MeasureText(button->label, 20);
    DrawText(button->label,
             button->rect.x + (button->rect.width - tw)/2,
             button->rect.y + (button->rect.height - 20)/2,
             20,
             button->text_color);
}

static void init_slider(Slider *slider, int x, int y, int length, int thickness)
{
    slider->bar = (Rectangle) {x, y, length, thickness};
    slider->handle = (Rectangle){ x + length/2 - thickness, y - thickness/2, thickness*2, thickness*2 };
    slider->volume_value = 0.5f;
    slider->dragging = false;
}

static void draw_slider(const Slider *slider)
{
    DrawRectangleRec(slider->bar, LIGHTGRAY);
    DrawRectangleRec(slider->handle, DARKGRAY);
}

Menu_action init_pause(Pause_menu *menu, int screen_w, int screen_h)
{
    menu->active = true;
    menu->action = MENU_NONE;

    int cx = screen_w/2;
    int cy = screen_h/2;

    // butoane

    InitButton(&menu->resume_button,  cx - 100, cy - 120, 200, 40,
               "RESUME GAME", DARKBLUE, BLUE, WHITE);
    InitButton(&menu->restart_button, cx - 100, cy - 60, 200, 40,
               "RESTART LEVEL", DARKGRAY, GRAY, WHITE);
    InitButton(&menu->exit_button,    cx - 100, cy + 0,   200, 40,
               "EXIT", RED, MAROON, WHITE);

    // slider pt volum
    init_slider(&menu->volume_slider, cx - 100, cy + 80, 200, 10);

    return MENU_NONE;
}

Menu_action UpdatePauseMenu(Pause_menu *menu)
{
    Vector2 mouse = GetMousePosition();
    menu->action = MENU_NONE;

    // resume
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, menu->resume_button.rect))
    {
        menu->action = MENU_RESUME;
        menu->active = false;
        return menu->action;
    }

    // restart
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, menu->restart_button.rect))
    {
        menu->action = MENU_RESTART;
        menu->active = false;
        return menu->action;
    }

    // exit
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, menu->exit_button.rect))
    {
        menu->action = MENU_EXIT;
        menu->active = false;
        return menu->action;
    }

    // slider
    Slider *sld = &menu->volume_slider;
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
        SetMasterVolume(sld->volume_value);
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) sld->dragging = false;

    return MENU_NONE;
}

void DrawPauseMenu(const Pause_menu *menu)
{
    
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

    
    const char *hdr = "MAIN MENU";
    int hw = MeasureText(hdr, 30);
    DrawText(hdr,
             GetScreenWidth()/2 - hw/2,
             GetScreenHeight()/2 - 180,
             30, YELLOW);

   
    DrawButton(&menu->resume_button);
    DrawButton(&menu->restart_button);
    DrawButton(&menu->exit_button);
    DrawSlider(&menu->volume_slider);
}

void UnloadPauseMenu(Pause_menu *menu)
{
    (void)menu;
}