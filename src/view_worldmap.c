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

static void map_stops_init(void);
void view_worldmap_init(view_Transition t) {
    memset(&view, 0, sizeof(view));
    view.ts_view_entered = RL_GetTime();

    view.ts_enter_anim_done = RL_GetTime();
    if (t.kind == view_TransitionKind_BackToWorldMap) {
        view.ts_enter_anim_done = RL_GetTime() + 1;
    }
    
    view.cam.pos.x = RL_GetScreenWidth()*0.5;
    view.cam.pos.y = RL_GetScreenHeight()*0.5;
    view.cam.zoom = 1.0f;

    map_stops_init();
}
void view_worldmap_free(void) {
}

static void map_stops_layout(void);
view_Transition view_worldmap_update(uint64_t _) {
    for (int i = 0; i < 100; i++)
        map_stops_layout();

    ui_update();
    return view.next_view;
}

typedef struct Stop Stop;
struct Stop {
    bool active;
    Stop *parent;

    uint32_t steps_from_root;

    ui_Icon icon;
    float x, y;
    size_t unit_count;
};

#define map_STOPS_MAX 400
static struct {
    Stop all[map_STOPS_MAX];
    Stop *start, *next;
} stops;

static Stop *map_stops_init_arm(Stop *base, int length, float angle);
static void map_stops_init(void) {
    stops.next = stops.all;

    Stop *start = stops.next++;
    *start = (Stop) {
        .active = true,
        .icon = ui_Icon_Camp,
        .x = 0,
        .y = 0,
        .steps_from_root = 0,
    };

    int arm_count = 3;
    for (int i = 0; i < arm_count; i++) {
        float jitter = 0;// 0.1 * (0.5f - randf());
        float t = ((float)i/(float)arm_count);
        float angle = M_PI*2.0f * t + jitter;
        Stop *end = map_stops_init_arm(start, RL_GetRandomValue(2, 3), angle);

        int arm_count = RL_GetRandomValue(2, 4);
        for (int i = 0; i < arm_count; i++) {
            float jitter = 0;// 0.1 * (0.5f - randf());
            float t = ((float)i/(float)arm_count);
            float angle = M_PI*2.0f * t + jitter;
            Stop *end2 = map_stops_init_arm(end, RL_GetRandomValue(2, 3), angle);

            int arm_count = RL_GetRandomValue(4, 5);
            for (int i = 0; i < arm_count; i++) {
                float jitter = 0;// 0.1 * (0.5f - randf());
                float t = ((float)i/(float)arm_count);
                float angle = M_PI*2.0f * t + jitter;
                Stop *end3 = map_stops_init_arm(end2, RL_GetRandomValue(2, 4), angle);

                int arm_count = RL_GetRandomValue(2, 3);
                for (int i = 0; i < arm_count; i++) {
                    float jitter = 0;// 0.1 * (0.5f - randf());
                    float t = ((float)i/(float)arm_count);
                    float angle = M_PI*2.0f * t + jitter;
                    map_stops_init_arm(end3, RL_GetRandomValue(1, 2), angle);
                }
            }
        }
    }
}

static void map_stops_layout(void) {

    for (size_t stop_i = 0; stop_i < countof(stops.all); stop_i++) {
        Stop *i = stops.all + stop_i;
        if (!i->active) continue;
        if (!i->parent) continue;

        /* my distance from 0, 0, should be roughly proportional
         * to the number of steps from me to the root node */
        float ideal_dist = i->steps_from_root * 100;
        float dist = sqrtf(i->x*i->x + i->y*i->y);
        float push = ideal_dist - dist;
        if (push > 0) {
            i->x += (i->x/dist) * push/2 * 0.001;
            i->y += (i->y/dist) * push/2 * 0.001;
        }
    }

    for (size_t stop_i = 0; stop_i < countof(stops.all); stop_i++) {
        Stop *i = stops.all + stop_i;
        Stop *p = i->parent;
        if (!i->active) continue;
        if (!i->parent) continue;

        float dx = p->x - i->x;
        float dy = p->y - i->y;
        float dist = sqrtf(dx*dx + dy*dy);
        float stretch = dist - 100;
        if (stretch > 0) {
            p->x -= (dx/dist) * stretch/2;
            p->y -= (dy/dist) * stretch/2;
            i->x += (dx/dist) * stretch/2;
            i->y += (dy/dist) * stretch/2;
        }
    }

    for (size_t stop_i = 0; stop_i < countof(stops.all); stop_i++) {
        Stop *i = stops.all + stop_i;
        if (!i->active) continue;

        for (size_t stop_j = stop_i + 1; stop_j < countof(stops.all); stop_j++) {
            Stop *j = stops.all + stop_j;
            if (!j->active) continue;

            float dx = j->x - i->x;
            float dy = j->y - i->y;
            float dist = sqrtf(dx*dx + dy*dy);
            float overlap = (100 + 100) - dist;
            if (overlap > 0) {
                j->x += (dx/dist) * overlap/2 * 0.01;
                j->y += (dy/dist) * overlap/2 * 0.01;
                i->x -= (dx/dist) * overlap/2 * 0.01;
                i->y -= (dy/dist) * overlap/2 * 0.01;
            }
        }
    }
}

static Stop *map_stops_init_arm(Stop *base, int length, float angle) {
    Stop *last = base;
    for (int i = 0; i < length; i++) {
        Stop *next = stops.next++;

        float dist = 100;
        float step_angle = angle + 0.1*(0.5 - randf());
        float fx = last->x + cosf(step_angle) * dist + 50*(0.5 - randf());
        float fy = last->y + sinf(step_angle) * dist + 50*(0.5 - randf());
        *next = (Stop) {
            .parent = last,
            .steps_from_root = last->steps_from_root + 1,
            .active = true,
            .icon = i%2 ? ui_Icon_Bed : ui_Icon_Swords,
            .x = fx,
            .y = fy,
        };
        last = next;
    }

    return last;
}

static bool map_stop_available(Stop *stop) {
    size_t parent_index = stop->parent - stops.all;
    return parent_index == (size_t)save.run.map_progress_idx;
}

static bool map_stop_complete(size_t index) {
    return false;
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
        .target = { 0, 0 },
        .rotation = 0,
        .zoom = 1.3f * view.cam.zoom,
    };
    RL_BeginMode2D(view.camera);

    RL_ClearBackground(RL_WHITE);

    for (size_t i = 0; i < countof(stops.all); i++) {
        Stop *stop = stops.all + i;
        if (!stop->active) continue;
        if (!stop->parent) continue;

        // RL_DrawLineBezier(
        RL_DrawLineEx(
            (RL_Vector2) { stop->x, stop->y },
            (RL_Vector2) { stop->parent->x, stop->parent->y },
            10,
            (RL_Color) { 119, 74, 82, 255 }
        );
    }

    for (size_t i = 0; i < countof(stops.all); i++) {
        Stop *stop = stops.all + i;
        if (!stop->active) continue;
        float x = stop->x;
        float y = stop->y;

        ui_Icon icon = stop->icon;
        float size = 30;

        Color tint = (Color){ 255, 255, 255, 255 };
        if (i == save.run.map_progress_idx) {
            icon = ui_Icon_Camp;

            float t = 1;
            if (stop->parent) {
                t = min(1, inv_lerp(
                        view.ts_view_entered,
                        view.ts_enter_anim_done,
                        RL_GetTime()
                ));
                x = lerp(stop->parent->x, x, t);
                y = lerp(stop->parent->y, y, t);
            }

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

        } else if (map_stop_available(stop))
            size *= 1.0f + 0.1*(1 + 0.5*sinf(RL_GetTime()*10));
        else if (map_stop_complete(i))
            tint = (Color) { 80, 80, 80, 180 };
        else if (map_stop_available(stop) == false)
            tint.a = 120;

        if (map_stop_available(stop)) {
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
    }

    RL_EndMode2D();

    ui_render(ui_create_layout());

    RL_EndDrawing();
}

static Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();

    return Clay_EndLayout(RL_GetFrameTime());
}
