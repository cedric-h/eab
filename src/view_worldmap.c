#include "raylib.h"
#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "svg.h"
#include "svg/path.h"

static struct {
    View next_view;
    RL_Camera2D camera;
    size_t current_stop_idx;
} view = {};

typedef struct {
    ui_Icon icon;
    float x, y;
} Stop;

Stop stops[] = {
    { ui_Icon_Camp,   129.6, 178.2 },
    { ui_Icon_Swords, 237.6, 178.2 },
    { ui_Icon_Bed,    316.4, 221.4 },
    { ui_Icon_Swords, 208.4, 259.2 },
    { ui_Icon_Bed,    132.8, 302.4 },
    { ui_Icon_Swords, 165.2, 399.6 },
    { ui_Icon_Swords, 221.4, 345.6 },
    { ui_Icon_Bed,    297.0, 345.6 },
    { ui_Icon_Swords, 297.0, 453.6 },
    { ui_Icon_Bed,    189.0, 480.6 },
    { ui_Icon_Swords, 210.6, 556.2 },
    { ui_Icon_Crown,  324.0, 556.2 },
};

static bool stop_available(size_t index) {
    return (index - view.current_stop_idx) == 1;
}

static bool stop_complete(size_t index) {
    return index <= view.current_stop_idx;
}

void view_worldmap_init(void) {
    size_t progress = view.current_stop_idx;
    memset(&view, 0, sizeof(view));
    view.current_stop_idx = progress;
}
void view_worldmap_free(void) {
}

View view_worldmap_update(void) {
    ui_update();
    return view.next_view;
}
static Clay_RenderCommandArray ui_create_layout(void);

typedef struct { float min_x, min_y, max_x, max_y; } Rect;
static void icon_draw(ui_Icon icon, Rect rect, Color tint) {
    RL_Texture t = *ui_icon(icon);

    RL_DrawTexturePro(
        t,
        (RL_Rectangle) { 0, 0, t.width, t.height },
        (RL_Rectangle) {
            rect.min_x,
            rect.min_y,
            rect.max_x - rect.min_x,
            rect.max_y - rect.min_y
        },
        (RL_Vector2) { 0, 0 },
        0,
        (RL_Color) { tint.r, tint.g, tint.b, tint.a }
    );
}

void view_worldmap_render(void) {
    RL_BeginDrawing();
    view.camera = (RL_Camera2D) {
        .offset = { RL_GetScreenWidth()*0.5, RL_GetScreenHeight()*0.5 },
        .target = { RL_GetScreenWidth()*0.5, RL_GetScreenHeight()*0.5 },
        .rotation = 0,
        .zoom = 1.3f,
    };
    RL_BeginMode2D(view.camera);

    RL_ClearBackground(RL_WHITE);

    svg_draw(&svg_path, (svg_Rect) {
        .min_x = 0,
        .min_y = 0,
        .max_x = RL_GetScreenWidth(),
        .max_y = RL_GetScreenHeight(),
    }, (Color) { 77, 63, 45, 255 } );

    for (size_t i = 0; i < countof(stops); i++) {
        Stop *stop = stops + i;

        ui_Icon icon = stop->icon;
        float size = 30;

        Color tint = (Color){ 255, 255, 255, 255 };
        if (i == view.current_stop_idx)
            icon = ui_Icon_Camp;
        else if (stop_available(i))
            size *= 1.0f + 0.1*(1 + 0.5*sinf(RL_GetTime()*10));
        else if (stop_complete(i))
            tint = (Color) { 80, 80, 80, 80 };
        else if (stop_available(i) == false)
            tint.a = 120;

        if (stop_available(i)) {
            RL_Vector2 m = RL_GetScreenToWorld2D(
                RL_GetMousePosition(),
                view.camera
            );
            float dist = sqrtf((m.x - stop->x)*(m.x - stop->x) +
                               (m.y - stop->y)*(m.y - stop->y));
            if (dist < size*0.5) {
                size *= 1.15;

                if (RL_IsMouseButtonPressed(0))
                    RL_PlaySound(ui_sound(ui_Sound_BattleEnter));

                if (RL_IsMouseButtonReleased(0))
                    view.next_view = View_Battle;
            }
        }

        icon_draw(
            icon,
            (Rect) {
                .min_x = stop->x - size/2,
                .max_x = stop->x + size/2,
                .min_y = stop->y - size/2,
                .max_y = stop->y + size/2
            },
            tint
        );
    }

    RL_EndMode2D();

    ui_render(ui_create_layout());

    RL_EndDrawing();
}

static Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();
    return Clay_EndLayout(RL_GetFrameTime());
}
