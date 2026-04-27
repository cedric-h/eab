#include "raylib.h"
#include "view.h"
#include "ui.h"
#include "save.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "svg.h"
#include "svg/path.h"
#include "draw.h"

static struct {
    double ts_view_entered, ts_enter_anim_done;
    view_Transition next_view;
    RL_Camera2D camera;

    /* don't do camera movement with the mouse, it's mine! */
    struct {
        f2 pos;
        float zoom;
        bool mouse_captured, moving_cam;
        f2 mouse_pos_down;
    } cam;
} view = {};

void view_worldmap_init(view_Transition t) {
    memset(&view, 0, sizeof(view));
    view.ts_view_entered = RL_GetTime();

    bool animate = (t.kind == view_TransitionKind_BackToWorldMap);
    view.ts_enter_anim_done = RL_GetTime() + (float)animate;
    
    view.cam.pos.x = RL_GetScreenWidth()*0.5;
    view.cam.pos.y = RL_GetScreenHeight()*0.5;
    view.cam.zoom = 1.0f;
}
void view_worldmap_free(void) {
}

view_Transition view_worldmap_update(uint64_t _) {
    ui_update();
    return view.next_view;
}

typedef struct {
    ui_Icon icon;
    float x, y;
    size_t unit_count;
} Stop;

Stop stops[] = {
    { ui_Icon_Bed,    129.6, 178.2,   0 },
    { ui_Icon_Swords, 237.6, 178.2,   6 },
    { ui_Icon_Bed,    316.4, 221.4,   0 },
    { ui_Icon_Swords, 208.4, 259.2,  12 },
    { ui_Icon_Bed,    132.8, 302.4,   0 },
    { ui_Icon_Swords, 165.2, 399.6,  18 },
    { ui_Icon_Swords, 221.4, 345.6,  18 },
    { ui_Icon_Bed,    297.0, 345.6,   0 },
    { ui_Icon_Swords, 297.0, 453.6,  40 },
    { ui_Icon_Bed,    189.0, 480.6,   0 },
    { ui_Icon_Swords, 210.6, 556.2,  80 },
    { ui_Icon_Crown,  324.0, 556.2, 100 },
};

static bool stop_available(size_t index) {
    return (index - (size_t)save.run.map_progress_idx) == 1;
}

static bool stop_complete(size_t index) {
    return index <= (size_t)save.run.map_progress_idx;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_worldmap_render(void) {

    float cx = view.cam.pos.x;
    float cy = view.cam.pos.y;
    if (!view.cam.mouse_captured) { /* free camera controls */
        RL_Vector2 m = RL_GetMousePosition();

        if (RL_IsMouseButtonPressed(0)) {
            view.cam.mouse_pos_down.x = m.x;
            view.cam.mouse_pos_down.y = m.y;
            view.cam.moving_cam = true;
        }
        if (RL_IsMouseButtonDown(0) && view.cam.moving_cam) {
            cx += m.x - view.cam.mouse_pos_down.x;
            cy += m.y - view.cam.mouse_pos_down.y;
        }
        if (RL_IsMouseButtonReleased(0) && view.cam.moving_cam) {
            view.cam.pos.x += m.x - view.cam.mouse_pos_down.x;
            view.cam.pos.y += m.y - view.cam.mouse_pos_down.y;
            cx = view.cam.pos.x;
            cy = view.cam.pos.y;
            view.cam.moving_cam = false;
        }

        view.cam.zoom += 0.02*RL_GetMouseWheelMoveV().y;
        view.cam.zoom = fabsf(view.cam.zoom);
    }

    /* this may get set back to true by the end of this function */
    view.cam.mouse_captured = false;

    RL_BeginDrawing();
    view.camera = (RL_Camera2D) {
        .offset = { cx, cy },
        .target = { RL_GetScreenWidth()*0.5, RL_GetScreenHeight()*0.5 },
        .rotation = 0,
        .zoom = 1.3f * view.cam.zoom,
    };
    RL_BeginMode2D(view.camera);

    RL_ClearBackground(RL_WHITE);

    svg_draw(&svg_path, (svg_Rect) {
        .min_x = 0,
        .min_y = 0,
        .max_x = RL_GetScreenWidth(),
        .max_y = RL_GetScreenHeight(),
    }, (Color) { 77, 63, 45, 255 } );

    float prev_x = 0, prev_y = 0;
    for (size_t i = 0; i < countof(stops); i++) {
        Stop *stop = stops + i;
        float x = stop->x;
        float y = stop->y;

        ui_Icon icon = stop->icon;
        float size = 30;

        Color tint = (Color){ 255, 255, 255, 255 };
        if (i == save.run.map_progress_idx) {
            icon = ui_Icon_Camp;
            float t = min(1, inv_lerp(
                    view.ts_view_entered,
                    view.ts_enter_anim_done,
                    RL_GetTime()
            ));
            x = lerp(prev_x, x, t);
            y = lerp(prev_y, y, t);

            draw_icon(
                stop->icon,
                (draw_Rect) {
                    .min_x = stop->x - size/2,
                    .max_x = stop->x + size/2,
                    .min_y = stop->y - size/2,
                    .max_y = stop->y + size/2
                },
                (Color) { 255, 255, 255, lerp(255, 0, t) }
            );

        } else if (stop_available(i))
            size *= 1.0f + 0.1*(1 + 0.5*sinf(RL_GetTime()*10));
        else if (stop_complete(i))
            tint = (Color) { 80, 80, 80, 180 };
        else if (stop_available(i) == false)
            tint.a = 120;

        if (stop_available(i)) {
            RL_Vector2 m = RL_GetScreenToWorld2D(
                RL_GetMousePosition(),
                view.camera
            );
            float dist = sqrtf((m.x - x)*(m.x - x) + (m.y - y)*(m.y - y));
            if (dist < size*0.5) {
                size *= 1.15;
                view.cam.mouse_captured = true;

                eab_mouse_cursor = MOUSE_CURSOR_POINTING_HAND;

                switch (stop->icon) {

                    case ui_Icon_Crown:
                    case ui_Icon_Swords: {
                        if (RL_IsMouseButtonPressed(0))
                            RL_PlaySound(ui_sound(ui_Sound_BattleEnter));
                        if (RL_IsMouseButtonReleased(0)) {
                            view.next_view.battle.unit_count = stop->unit_count;
                            view.next_view.kind = view_TransitionKind_StartBattle;
                        }
                    } break;

                    case ui_Icon_Bed: {
                        if (RL_IsMouseButtonPressed(0))
                            RL_PlaySound(ui_sound(ui_Sound_CampEnter));
                        if (RL_IsMouseButtonReleased(0))
                            view.next_view.kind = view_TransitionKind_StartCamp;
                    } break;

                    default:
                        assert(false);
                        break;

                }
            }
        }

        draw_icon(
            icon,
            (draw_Rect) {
                .min_x = x - size/2,
                .max_x = x + size/2,
                .min_y = y - size/2,
                .max_y = y + size/2
            },
            tint
        );
        prev_x = x;
        prev_y = y;
    }

    RL_EndMode2D();

    ui_render(ui_create_layout());

    RL_EndDrawing();
}

static Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();

    return Clay_EndLayout(RL_GetFrameTime());
}
