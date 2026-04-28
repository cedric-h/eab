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

typedef enum {
    map_Biome_Plains,
    map_Biome_Forest,
    map_Biome_DarkForest,
    map_Biome_Desert,
    map_Biome_COUNT,
} map_Biome;

static Color map_biome_color[] = {
    [map_Biome_Plains    ] = { 104, 148, 122, 255 },
    [map_Biome_Forest    ] = {  80, 109,  92, 255 },
    [map_Biome_DarkForest] = {  55,  67,  60, 255 },
    [map_Biome_Desert    ] = { 229, 196, 163, 255 },
};
_Static_assert(
    countof(map_biome_color) == map_Biome_COUNT,
    "missing biome color"
);

typedef struct Stop Stop;
typedef enum {
    map_StopStage_NONE,
    map_StopStage_New,
    map_StopStage_Visited,
} map_StopStage;
struct Stop {
    map_StopStage stage;
    Stop *parent;

    uint16_t steps_from_root;
    map_Biome biome;

    ui_Icon icon;
    float x, y;

    struct {
        uint16_t variant;
        f2 pos;
    } assets[3];
};

#define map_STOPS_MAX 400
static struct {
    bool inited;

    Stop all[map_STOPS_MAX];
    Stop *start, *current, *previous;

    /* next stop to allocate */
    Stop *next;
} stops;

#define map_MAX_ASSETS_PER_BIOME 10
static struct {
    double ts_enter_anim_start, ts_enter_anim_done;
    view_Transition next_view;
    RL_Camera2D camera;

    struct {
        size_t texture_count;
        RL_Texture textures[map_MAX_ASSETS_PER_BIOME];
    } biome_art[map_Biome_COUNT];

    struct {
        f2 pos;
        float zoom;

        /* don't do camera movement with the mouse, it's mine! */
        bool mouse_captured;

        /* this mouse movement is being used for camera movement */
        bool moving_cam;

        f2 mouse_pos_down;
    } cam;
} view = {};

static void map_stops_init(void);
static void map_stops_layout(void);
static void map_biome_art_init(void);
void view_worldmap_init(view_Transition t) {
    memset(&view, 0, sizeof(view));

    view.ts_enter_anim_start = RL_GetTime();
    view.ts_enter_anim_done = RL_GetTime();
    if (t.kind == view_TransitionKind_BackToWorldMap) {
        view.ts_enter_anim_done = RL_GetTime() + 1;
    }
    
    map_biome_art_init();

    if (!stops.inited) {
        stops.inited = true;

        map_stops_init();
        for (int i = 0; i < 10000; i++) map_stops_layout();
    }

    view.cam.pos.x = RL_GetScreenWidth()*0.5 - stops.current->x;
    view.cam.pos.y = RL_GetScreenHeight()*0.5 - stops.current->y;
    view.cam.zoom = 1.0f;
}
static void map_biome_art_init(void) {
    struct {
        char *paths[map_MAX_ASSETS_PER_BIOME];
    } assets_for_biome[map_Biome_COUNT] = {
        [map_Biome_DarkForest] = {
            "darkforest_env1.png",
            "darkforest_env2.png",
            "darkforest_env3.png",
            "darkforest_env4.png",
            "darkforest_env5.png",
            "darkforest_env6.png",
        },
        [map_Biome_Desert] = {
            "desert_env1.png",
            "desert_env2.png",
            "desert_env3.png",
            "desert_env4.png",
        },
        [map_Biome_Forest] = {
            "forest_env1.png",
            "forest_env2.png",
            "forest_env3.png",
            "forest_env4.png",
            "forest_env5.png",
            "forest_env6.png",
            "forest_env7.png",
        },
        [map_Biome_Plains] = {
            "forest_env1.png",
            "forest_env2.png",
            "forest_env3.png",
            "forest_env4.png",
            "forest_env5.png",
            "forest_env6.png",
            "forest_env7.png",
        },
    };

    for (map_Biome biome = 0; biome < map_Biome_COUNT; biome++) {
        for (int i = 0; i < map_MAX_ASSETS_PER_BIOME; i++) {
            char *asset = assets_for_biome[biome].paths[i];
            if (asset == NULL) continue;

            char path[50] = {0};
            snprintf(path, sizeof(path) - 1, "resources/env/%s", asset);
            RL_Texture t = RL_LoadTexture(path);
            RL_SetTextureFilter(
                t,
                TEXTURE_FILTER_BILINEAR
            );
            view.biome_art[biome].textures[i] = t;
            view.biome_art[biome].texture_count += 1;
        }
    }
}

void view_worldmap_free(void) {
    for (map_Biome biome = 0; biome < map_Biome_COUNT; biome++) {
        for (
            size_t i = 0;
            i < view.biome_art[biome].texture_count;
            i++
        ) {
            RL_UnloadTexture(view.biome_art[biome].textures[i]);
        }
    }
}

view_Transition view_worldmap_update(uint64_t _) {
    // for (int i = 0; i < 100; i++) map_stops_layout();

    ui_update();
    return view.next_view;
}

static void map_stops_assign_assets(Stop *stop) {
    for (size_t i = 0; i < countof(stop->assets); i++) {
        // float x = lerpf(30, 80, randf()) * ((randf() < 0.5) ? -1 : 1);
        // float y = lerpf(30, 80, randf()) * ((randf() < 0.5) ? -1 : 1);
        float t = (float)i / (float)countof(stop->assets);
        float x = cosf(t*M_PI*2.0) * 60; // lerpf(50, 100, randf());
        float y = sinf(t*M_PI*2.0) * 60; // lerpf(50, 100, randf());
        stop->assets[i].pos.x = x;
        stop->assets[i].pos.y = y;
        stop->assets[i].variant = RL_GetRandomValue(
            0,
            view.biome_art[stop->biome].texture_count
        );
    }
}

static Stop *map_stops_init_arm(Stop *base, map_Biome biome, int length, float angle);
static void map_stops_init(void) {
    stops.next = stops.all;

    Stop *start = stops.next++;
    *start = (Stop) {
        .stage = map_StopStage_New,
        .icon = ui_Icon_BackToMap,
        .biome = map_Biome_Plains,
        .steps_from_root = 0,
        .x = 0,
        .y = 0,
    };
    map_stops_assign_assets(start);
    stops.start = start;
    stops.previous = start;
    stops.current = start;

    int arm_count = map_Biome_COUNT;
    for (int i = 1; i < arm_count; i++) {
        map_Biome biome = i;
        float jitter = 0;// 0.1 * (0.5f - randf());

        float t0 = (((float)i - 0.45f)/(float)(arm_count - 1));
        float t1 = (((float)i + 0.45f)/(float)(arm_count - 1));
        float angle0 = M_PI*2.0f * t0 + jitter;
        float angle1 = M_PI*2.0f * t1 + jitter;
        Stop *end = map_stops_init_arm(
            start,
            biome,
            RL_GetRandomValue(1, 2),
            lerp_rads(angle0, angle1, 0.5)
        );

        int arm_count = RL_GetRandomValue(3, 5);
        for (int i = 0; i < arm_count; i++) {

            float imax = arm_count - 1;
            float _0angle0 = lerp_rads(angle0, angle1, (float)(i - 0.5f)/imax);
            float _0angle1 = lerp_rads(angle0, angle1, (float)(i + 0.5f)/imax);
            Stop *end0 = map_stops_init_arm(
                end,
                biome,
                RL_GetRandomValue(1, 3),
                lerp_rads(_0angle0, _0angle1, 0.5)
            );

            int arm_count = RL_GetRandomValue(3, 5);
            for (int i = 0; i < arm_count; i++) {
                float imax = arm_count - 1;
                float _1angle0 = lerp_rads(_0angle0, _0angle1, (float)(i - 0.5f)/imax);
                float _1angle1 = lerp_rads(_0angle0, _0angle1, (float)(i + 0.5f)/imax);
                map_stops_init_arm(
                    end0,
                    biome,
                    RL_GetRandomValue(0, 3),
                    lerp_rads(_1angle0, _1angle1, 0.5)
                );
            }
        }
    }
}

static void map_stops_layout(void) {

    for (size_t stop_i = 0; stop_i < countof(stops.all); stop_i++) {
        Stop *i = stops.all + stop_i;
        Stop *p = i->parent;
        if (!i->stage) continue;
        if (!i->parent) continue;

        float dx = p->x - i->x;
        float dy = p->y - i->y;
        float dist = sqrtf(dx*dx + dy*dy);
        float stretch = dist - 100;
        if (stretch > 0) {
            p->x -= (dx/dist) * stretch/2 * 0.1;
            p->y -= (dy/dist) * stretch/2 * 0.1;
            i->x += (dx/dist) * stretch/2 * 0.1;
            i->y += (dy/dist) * stretch/2 * 0.1;
        }
    }

    for (size_t stop_i = 0; stop_i < countof(stops.all); stop_i++) {
        Stop *i = stops.all + stop_i;
        if (!i->stage) continue;

        for (size_t stop_j = stop_i + 1; stop_j < countof(stops.all); stop_j++) {
            Stop *j = stops.all + stop_j;
            if (!j->stage) continue;

            float dx = j->x - i->x;
            float dy = j->y - i->y;
            float dist = sqrtf(dx*dx + dy*dy);
            float overlap = (100 + 100) - dist;
            if (overlap > 0) {
                j->x += (dx/dist) * overlap/2 * 0.1;
                j->y += (dy/dist) * overlap/2 * 0.1;
                i->x -= (dx/dist) * overlap/2 * 0.1;
                i->y -= (dy/dist) * overlap/2 * 0.1;
            }
        }
    }
}

static Stop *map_stops_init_arm(Stop *base, map_Biome biome, int length, float angle) {
    Stop *last = base;
    for (int i = 0; i < length; i++) {
        Stop *next = stops.next++;

        float dist = 100;
        float step_angle = angle + 0.1*(0.5 - randf());
        float fx = last->x + cosf(step_angle) * dist;// + 50*(0.5 - randf());
        float fy = last->y + sinf(step_angle) * dist;// + 50*(0.5 - randf());
        *next = (Stop) {
            .parent = last,
            .steps_from_root = last->steps_from_root + 1,
            .stage = map_StopStage_New,
            .x = fx,
            .y = fy,
        };

        next->biome = (next->steps_from_root < 3)
            ? map_Biome_Plains
            : biome;
        map_stops_assign_assets(next);

        next->icon = (randf() < 0.2)
            ? ui_Icon_Swords
            : ui_Icon_Bed;
        if (next->parent->icon == ui_Icon_Bed || 
            next->parent->icon == ui_Icon_BackToMap)
            next->icon = ui_Icon_Swords;

        last = next;
    }

    return last;
}

static bool map_stop_available(Stop *stop) {
    return stop->parent == stops.current || stops.current->parent == stop;
}

static bool map_stop_complete(Stop *stop) {
    return stop->stage == map_StopStage_Visited;
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
        .zoom = 0.9f * view.cam.zoom,
    };
    RL_BeginMode2D(view.camera);

    RL_ClearBackground((RL_Color) { 97, 131, 161, 255 });

    map_Biome biome_render_order[map_Biome_COUNT] = {
        map_Biome_Desert,
        map_Biome_DarkForest,
        map_Biome_Forest,
        map_Biome_Plains,
    };
    for (int i = 0; i < map_Biome_COUNT; i++) {
        map_Biome b = biome_render_order[i];

        for (size_t i = 0; i < countof(stops.all); i++) {
            Stop *stop = stops.all + i;
            if (!stop->stage) break;
            if (stop->biome != b) continue;

            RL_DrawCircle(
                stop->x,
                stop->y,
                140.0f,
                (RL_Color) {
                    map_biome_color[stop->biome].r,
                    map_biome_color[stop->biome].g,
                    map_biome_color[stop->biome].b,
                    map_biome_color[stop->biome].a,
                }
            );
        }
    }

    for (size_t s = 0; s < countof(stops.all); s++) {
        Stop *stop = stops.all + s;
        if (!stop->stage) continue;

        for (size_t i = 0; i < countof(stop->assets); i++) {
            RL_Texture t = view
                .biome_art[stop->biome]
                .textures[stop->assets[i].variant];

            // RL_DrawLineBezier(
            float scale = 0.1;
            RL_DrawTextureEx(
                t,
                (RL_Vector2) {
                    stop->x + stop->assets[i].pos.x - t.width*0.5*scale,
                    stop->y + stop->assets[i].pos.y - t.height*0.8*scale,
                },
                0,
                scale,
                (RL_Color) { 255, 255, 255, 75 }
            );

            if (0) RL_DrawCircle(
                stop->x + stop->assets[i].pos.x,
                stop->y + stop->assets[i].pos.y,
                10,
                (RL_Color) { 255, 0, 0, 255 }
            );
        }
    }

    for (size_t i = 0; i < countof(stops.all); i++) {
        Stop *stop = stops.all + i;
        if (!stop->stage) continue;
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
        if (!stop->stage) continue;
        float x = stop->x;
        float y = stop->y;

        ui_Icon icon = stop->icon;
        float size = 65;

        Color tint = (Color){ 255, 255, 255, 255 };
        if (stops.current == stop) {
            icon = ui_Icon_Camp;

            float t = 1;
            if (stops.previous != stop) {
                t = min(1, inv_lerp(
                        view.ts_enter_anim_start,
                        view.ts_enter_anim_done,
                        RL_GetTime()
                ));
                x = lerp(stops.previous->x, x, t);
                y = lerp(stops.previous->y, y, t);
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

        }
        else if (map_stop_complete(stop))
            icon = ui_Icon_Grave;
        else if (map_stop_available(stop))
            size *= 1.0f + 0.1*(1 + 0.5*sinf(RL_GetTime()*10));
        else if (map_stop_available(stop) == false)
            size *= 0.7, tint = (Color) { 120, 120, 120, 255 };

        if (map_stop_available(stop)) {
            RL_Vector2 m = RL_GetScreenToWorld2D(
                RL_GetMousePosition(),
                view.camera
            );
            float dist = sqrtf((m.x - x)*(m.x - x) + (m.y - y)*(m.y - y));
            if (dist < size*0.5) do {
                size *= 1.15;
                view.cam.mouse_captured = true;

                eab_mouse_cursor = MOUSE_CURSOR_POINTING_HAND;

                if (RL_IsMouseButtonPressed(0) && map_stop_complete(stop)) {
                    view.ts_enter_anim_start = RL_GetTime();
                    view.ts_enter_anim_done = RL_GetTime() + 1;
                    stops.current->stage = map_StopStage_Visited;
                    stops.previous = stops.current;
                    stops.current = stop;
                    break;
                }

                switch (stop->icon) {

                    case ui_Icon_Crown:
                    case ui_Icon_Swords: {
                        if (RL_IsMouseButtonPressed(0))
                            RL_PlaySound(ui_sound(ui_Sound_BattleEnter));
                        if (RL_IsMouseButtonReleased(0)) {
                            stops.current->stage = map_StopStage_Visited;
                            stops.previous = stops.current;
                            stops.current = stop;
                            view.next_view.battle.unit_count = 4 * stop->steps_from_root;
                            view.next_view.kind = view_TransitionKind_StartBattle;
                        }
                    } break;

                    case ui_Icon_Bed: {
                        if (RL_IsMouseButtonPressed(0))
                            RL_PlaySound(ui_sound(ui_Sound_CampEnter));
                        if (RL_IsMouseButtonReleased(0)) {
                            stops.current->stage = map_StopStage_Visited;
                            stops.previous = stops.current;
                            stops.current = stop;
                            view.next_view.kind = view_TransitionKind_StartCamp;
                        }
                    } break;

                    case ui_Icon_BackToMap:
                        break;

                    default:
                        assert(false);
                        break;

                }
            } while (false);
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
