#include "raylib.h"
#include "view.h"
#include "ui.h"
#include "save.h"
#include "draw.h"
#include "guy.h"
#include <math.h>
#include <stdio.h>

#include "svg.h"
#include "svg/orgy_circle.h"

#define ORGY_CIRCLE_X    175
#define ORGY_CIRCLE_Y    405
#define ORGY_CIRCLE_SIZE 250 /* radius */

#define RECYCLER_X 380
#define RECYCLER_Y 250
#define RECYCLER_SIZE 50

typedef enum {
    camp_ItemKind_NONE,
    camp_ItemKind_Furniture,
    camp_ItemKind_Guy,
} camp_ItemKind;
typedef struct {
    camp_ItemKind kind;
    union {
        save_Furniture furniture;

        /* TODO: generational indexing here so we can know when a guy is reused */
        guy_Guy *guy;
    };
    struct { float x, y; } pos;
} camp_Item;

static struct {
    size_t guys_in_orgy_circle_male;
    size_t guys_in_orgy_circle_female;

    view_Transition next_view;
    int held_item_idx;
    RL_Sound stew[2];
} view = {};

/* not in view because it's kept across view changes while view is reset */
static struct {
    uint32_t run_id;
    camp_Item items[countof(save.run.furniture) + countof(save.run.guys)];
} keep;

static camp_Item *camp_get_item(camp_ItemKind kind, size_t idx) {
    switch (kind) {
        case camp_ItemKind_NONE:
            return NULL;
        case camp_ItemKind_Furniture:
            return keep.items + idx;
        case camp_ItemKind_Guy:
            return keep.items + countof(save.run.furniture) + idx;
    }
}

static float camp_get_item_size(camp_Item *item) {
    switch (item->kind) {
        case camp_ItemKind_NONE:
            return 0;
        case camp_ItemKind_Furniture:
            return 25;
        case camp_ItemKind_Guy:
            return 26*guy_size(item->guy);
    }
}

static camp_Item camp_make_item(camp_ItemKind kind) {
    float w = RL_GetScreenWidth();
    float h = RL_GetScreenHeight();
    float x = RL_GetRandomValue(w*0.2, w*0.8);
    float y = RL_GetRandomValue(h*0.2, h*0.8);

    /* push them out of the orgy circle */
    {
        float dist = sqrtf((x - ORGY_CIRCLE_X)*(x - ORGY_CIRCLE_X) +
                           (y - ORGY_CIRCLE_Y)*(y - ORGY_CIRCLE_Y));
        if (dist < ORGY_CIRCLE_SIZE/2) {
            x = ORGY_CIRCLE_X + (x - ORGY_CIRCLE_X)/dist * ORGY_CIRCLE_SIZE;
            y = ORGY_CIRCLE_Y + (y - ORGY_CIRCLE_Y)/dist * ORGY_CIRCLE_SIZE;
        }
    }

    return (camp_Item) {
        .kind = kind,
        .pos = { x, y }
    };
}

void view_camp_init(view_Transition t) {
    memset(&view, 0, sizeof(view));
    view.held_item_idx = -1;

    if (save.run.id != keep.run_id) {
        memset(&keep, 0, sizeof(keep));
        keep.run_id = save.run.id;
    }

    view.stew[0] = RL_LoadSound("./resources/audio/stew1.wav");
    view.stew[1] = RL_LoadSound("./resources/audio/stew2.wav");

    /* find items added to the save since last camp and initialize them */
    {
        for (size_t furn_i = 0; furn_i < countof(save.run.furniture); furn_i++) {
            save_Furniture f = save.run.furniture[furn_i];
            if (f == save_Furniture_NONE) continue;

            camp_Item *i = camp_get_item(camp_ItemKind_Furniture, furn_i);
            if (i->kind == camp_ItemKind_NONE) {
                *i = camp_make_item(camp_ItemKind_Furniture);
                i->furniture = f;
            }
        }

        for (size_t guy_i = 0; guy_i < countof(save.run.guys); guy_i++) {
            guy_Guy *g = save.run.guys + guy_i;
            if (g->state == guy_GuyState_NONE) continue;

            camp_Item *i = camp_get_item(camp_ItemKind_Guy, guy_i);

            /* TODO: check if generational index is different and reset if this
             * guy's memory has been reused */

            if (i->kind == camp_ItemKind_NONE) {
                *i = camp_make_item(camp_ItemKind_Guy);
                i->guy = g;
            }
        }
    }
}
void view_camp_free(void) {
    RL_UnloadSound(view.stew[0]);
    RL_UnloadSound(view.stew[1]);
}

/* construct the "to_fornications" view transition */
static view_Transition to_fornications(void) {
    view_Transition ret = {
        .kind = view_TransitionKind_CampFornications,
    };

    for (size_t i = 0; i < countof(keep.items); i++) {
        camp_Item *item = keep.items + i;
        if (item->kind == camp_ItemKind_NONE) continue;
        float from_center =
            sqrtf((ORGY_CIRCLE_X - item->pos.x)*(ORGY_CIRCLE_X - item->pos.x) +
                  (ORGY_CIRCLE_Y - item->pos.y)*(ORGY_CIRCLE_Y - item->pos.y));

        if (from_center > (ORGY_CIRCLE_SIZE/2))
            continue;

        if (item->kind == camp_ItemKind_Guy) {
            for (size_t j = 0; j < countof(ret.fornications.in_orgy); j++) {
                if (ret.fornications.in_orgy[j] != NULL)
                    continue;
                ret.fornications.in_orgy[j] = item->guy;
                break;
            }
        }
    }
    
    return ret;
}

view_Transition view_camp_update(void) {

    /* push things in/out of the orgy circle */
    view.guys_in_orgy_circle_male = 0;
    view.guys_in_orgy_circle_female = 0;
    for (size_t i = 0; i < countof(keep.items); i++) {
        camp_Item *item = keep.items + i;
        if (item->kind == camp_ItemKind_NONE) continue;
        float from_center =
            sqrtf((ORGY_CIRCLE_X - item->pos.x)*(ORGY_CIRCLE_X - item->pos.x) +
                  (ORGY_CIRCLE_Y - item->pos.y)*(ORGY_CIRCLE_Y - item->pos.y));

        if (from_center < (ORGY_CIRCLE_SIZE/2)) {
            if (item->kind == camp_ItemKind_Guy) {
                view.guys_in_orgy_circle_male   += item->guy->sex == guy_Sex_Male;
                view.guys_in_orgy_circle_female += item->guy->sex == guy_Sex_Female;
            }
        }
        float from_edge = (ORGY_CIRCLE_SIZE/2 - 5) - from_center;
        float from_edge_min = 50;
        if (fabsf(from_edge) < from_edge_min) {
            float out_x = (item->pos.x - ORGY_CIRCLE_X) / from_center;
            float out_y = (item->pos.y - ORGY_CIRCLE_Y) / from_center;
            item->pos.x += out_x * (from_edge - from_edge_min*sign(from_edge))*0.4;
            item->pos.y += out_y * (from_edge - from_edge_min*sign(from_edge))*0.4;
        }
    }

    /* push things out of the guy recycler */
    for (size_t item_j = 0; item_j < countof(keep.items); item_j++) {
        camp_Item *j = keep.items + item_j;
        if (j->kind == camp_ItemKind_NONE) continue;

        float dx = j->pos.x - RECYCLER_X;
        float dy = j->pos.y - RECYCLER_Y;
        float dist = sqrtf(dx*dx + dy*dy);
        float overlap = (camp_get_item_size(j) + RECYCLER_SIZE) - dist;
        if (overlap > 0) {
            j->pos.x += (dx/dist) * overlap*0.5;
            j->pos.y += (dy/dist) * overlap*0.5;
        }
    }

    /* push items out of each other */
    for (size_t item_i = 0; item_i < countof(keep.items); item_i++) {
        camp_Item *i = keep.items + item_i;
        if (i->kind == camp_ItemKind_NONE) continue;

        for (size_t item_j = item_i + 1; item_j < countof(keep.items); item_j++) {
            camp_Item *j = keep.items + item_j;
            if (j->kind == camp_ItemKind_NONE) continue;

            float dx = j->pos.x - i->pos.x;
            float dy = j->pos.y - i->pos.y;
            float dist = sqrtf(dx*dx + dy*dy);
            float overlap = (camp_get_item_size(i) + camp_get_item_size(j)) - dist;
            if (overlap > 0) {
                j->pos.x += (dx/dist) * overlap/2;
                j->pos.y += (dy/dist) * overlap/2;
                i->pos.x -= (dx/dist) * overlap/2;
                i->pos.y -= (dy/dist) * overlap/2;
            }
        }
    }

    /* keep them on the screen */
    for (size_t item_j = 0; item_j < countof(keep.items); item_j++) {
        camp_Item *j = keep.items + item_j;
        if (j->kind == camp_ItemKind_NONE) continue;

        float w = RL_GetScreenWidth();
        float h = RL_GetScreenHeight();
        j->pos.x = max(w*0.2, min(w*0.8, j->pos.x));
        j->pos.y = max(h*0.2, min(h*0.8, j->pos.y));
    }

    ui_update();
    return view.next_view;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_camp_render(void) {
    RL_BeginDrawing();

    RL_ClearBackground(RL_WHITE);

    /* held things go to mouse */
    if (view.held_item_idx != -1) {
        camp_Item *item = keep.items + view.held_item_idx;
        RL_Vector2 m = RL_GetMousePosition();
        item->pos.x = m.x;
        item->pos.y = m.y;
    }

    /* draw guy recycler */
    bool over_guy_recyler = false;
    {
        float x = RECYCLER_X;
        float y = RECYCLER_Y;
        float size = RECYCLER_SIZE + 10;

        ui_Font font = ui_Font_Desc;
        RL_DrawTextEx(
            ui_font_rl(font),
            "guy",
            (RL_Vector2) { x - size*0.4, y - size*0.8 },
            ui_font_size(font),
            1,
            (RL_Color) { 0, 0, 0, 255 }
        );
        RL_DrawTextEx(
            ui_font_rl(font),
            "recyler",
            (RL_Vector2) { x - size*0.7, y + size/2 },
            ui_font_size(font),
            1,
            (RL_Color) { 0, 0, 0, 255 }
        );

        bool holding_guy = (view.held_item_idx != -1) &&
            (keep.items[view.held_item_idx].kind == camp_ItemKind_Guy);

        {
            float icon_size = size;
            float t = 0;
            if (holding_guy) {
                t = fabsf(sinf(RL_GetTime()*7));
            }

            RL_Vector2 m = RL_GetMousePosition();
            float dist = sqrtf((x - m.x)*(x - m.x) +
                               (y - m.y)*(y - m.y));
            if (holding_guy) {
                float t = inv_lerp(size, size/2, dist - size);
                icon_size *= 1.0 + 0.3*min(1, max(0, t));
            }

            over_guy_recyler = dist < (size/2);

            float ix = x;
            float iy = y - 10*t;
            float xsize = icon_size + 5*t;
            float ysize = icon_size - 5*t;
            draw_icon(
                ui_Icon_Soup,
                (draw_Rect) {
                    .min_x = ix - xsize/2,
                    .min_y = iy - ysize/2,
                    .max_x = ix + xsize/2,
                    .max_y = iy + ysize/2,
                },
                (Color) { 255, 255, 255, 255 }
            );
        }
    }

    /* find thing closest to mouse */
    camp_Item *item_closest_mouse = NULL;
    float item_closest_mouse_dist = 30.0f;
    for (size_t i = 0; i < countof(keep.items); i++) {
        camp_Item *item = keep.items + i;
        if (item->kind == camp_ItemKind_NONE) continue;

        RL_Vector2 m = RL_GetMousePosition();
        float dist = sqrtf((item->pos.x - m.x)*(item->pos.x - m.x) +
                           (item->pos.y - m.y)*(item->pos.y - m.y));
        if (dist < item_closest_mouse_dist) {
            item_closest_mouse_dist = dist;
            item_closest_mouse = item;
        }
    }

    /* draw items */
    for (size_t i = 0; i < countof(keep.items); i++) {
        camp_Item *item = keep.items + i;
        if (item->kind == camp_ItemKind_NONE) continue;

        float scale = 1;
        float rotation = 0;

        if (item_closest_mouse == item && view.held_item_idx == -1) {
            scale *= 1.05;
            rotation += sinf(RL_GetTime()*15)*5.0f;

            eab_mouse_cursor = MOUSE_CURSOR_POINTING_HAND;
            if (RL_IsMouseButtonPressed(0))
                view.held_item_idx = i;
        }


        RL_BeginMode2D((RL_Camera2D) {
            .offset = { item->pos.x, item->pos.y },
            .target = { item->pos.x, item->pos.y },
            .rotation = rotation,
            .zoom = scale,
        });
        switch (item->kind) {
            case camp_ItemKind_NONE: break;

            case camp_ItemKind_Furniture: {
                float size = 50.0f * scale;
                save_FurnitureConfig *fc = save_furniture_configs + item->furniture;

                draw_icon(
                    fc->icon,
                    (draw_Rect) {
                        .min_x = item->pos.x - size/2,
                        .min_y = item->pos.y - size/2,
                        .max_x = item->pos.x + size/2,
                        .max_y = item->pos.y + size/2,
                    },
                    (Color) { 255, 255, 255, 255 }
                );
            }; break;

            case camp_ItemKind_Guy: {
                guy_DrawFlags flags = 0;
                if (item_closest_mouse == item) flags |= guy_DrawFlags_Name;
                guy_draw(item->guy, item->pos.x, item->pos.y, flags);
            }; break;
        }
        RL_EndMode2D();
    }


    /* draw orgy circle */
    {
        float x = ORGY_CIRCLE_X;
        float y = ORGY_CIRCLE_Y;
        float size = ORGY_CIRCLE_SIZE;
        RL_Vector2 m = RL_GetMousePosition();
        bool hover = sqrtf((x - m.x)*(x - m.x) + (y - m.y)*(y - m.y)) < (size/2);
        hover = hover && (view.held_item_idx != -1);
        svg_draw(&svg_orgy_circle, (svg_Rect) {
            .min_x = x-size/2,
            .min_y = y-size/2,
            .max_x = x+size/2,
            .max_y = y+size/2,
        }, (Color) { hover ? 155 : 0, 0, 0, 255 } );

        ui_Font font = ui_Font_Desc;
        RL_DrawTextEx(
            ui_font_rl(font),
            "the orgy circle",
            (RL_Vector2) { x + 10 - size/2, y - 30 - size/2 },
            ui_font_size(font),
            1,
            (RL_Color) { 0, 0, 0, 255 }
        );
    }

    ui_render(ui_create_layout());

    RL_EndDrawing();

    if (RL_IsMouseButtonReleased(0)) {
        if (over_guy_recyler &&
            (view.held_item_idx != -1) &&
            (keep.items[view.held_item_idx].kind == camp_ItemKind_Guy)
        ) {
            camp_Item *i = keep.items + view.held_item_idx;
            i->guy->state = guy_GuyState_NONE;
            i->kind = camp_ItemKind_NONE;
            save.run.food += 1;
            RL_PlaySound(view.stew[
                (size_t)floorf(RL_GetRandomValue(0, countof(view.stew) - 1))
            ]);
        }

        view.held_item_idx = -1;
    }
}

static Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0)
            },
            .padding = { 32, 32, 32, 32 },
            .childGap = 16,
        },
        .backgroundColor = {0}
    }) {

        CLAY(CLAY_ID("title"), {
            .layout = {
                .padding = { 0, 0, 0, 24 },
                .sizing = { .width = CLAY_SIZING_GROW() },
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
            }
        }) {
            CLAY_TEXT(
                CLAY_STRING("camp"),
                ui_font(ui_Font_SubTitle),
            );

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
            });

            CLAY_AUTO_ID({
                .layout = {
                    .sizing = {
                        .height = CLAY_SIZING_FIXED(32),
                        .width = CLAY_SIZING_FIXED(32),
                    },
                },
                .image = { .imageData = ui_icon(ui_Icon_Food) }
            });

            Clay_String tmp;
            ui_sprintf(tmp, "x%d", save.run.food);
            CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
        }

        CLAY_AUTO_ID({
            .layout = { .sizing = { .height = CLAY_SIZING_GROW() } }
        });

        CLAY_AUTO_ID({
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW() },
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            },
        }) {
            uint32_t map_cost = 0;
            uint32_t bed_cost = view.guys_in_orgy_circle_female*2 * (view.guys_in_orgy_circle_male > 0);
            for (size_t i = 0; i < countof(save.run.guys); i++) {
                if (save.run.guys[i].state == guy_GuyState_NONE)
                    continue;
                map_cost += 1;
            }

            CLAY_AUTO_ID({
                .layout = {
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
                    .padding = { 6, 6, 0, 0 },
                    .sizing = { .width = CLAY_SIZING_GROW() },
                }
            }) {
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = {
                            .height = CLAY_SIZING_FIXED(32),
                            .width = CLAY_SIZING_FIXED(32),
                        },
                    },
                    .image = { .imageData = ui_icon(ui_Icon_Food) }
                });

                CLAY_AUTO_ID({
                    .layout = { .sizing = { .width = CLAY_SIZING_FIXED(10) } }
                });

                {
                    Clay_String tmp;
                    ui_sprintf(tmp, "x%u", bed_cost);
                    CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
                }

                CLAY_AUTO_ID({
                    .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
                });

                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = {
                            .height = CLAY_SIZING_FIXED(32),
                            .width = CLAY_SIZING_FIXED(32),
                        },
                    },
                    .image = { .imageData = ui_icon(ui_Icon_Food) }
                });

                CLAY_AUTO_ID({
                    .layout = { .sizing = { .width = CLAY_SIZING_FIXED(10) } }
                });

                {
                    Clay_String tmp;
                    ui_sprintf(tmp, "x%d", map_cost);
                    CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
                }
            }

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } },
            }) {
                switch (ui_small_button(
                        ui_icon(ui_Icon_Bed),
                        (bed_cost == 0) || (save.run.food < bed_cost)
                    )) {
                    case ui_Click_Pressed: {
                        RL_PlaySound(ui_sound(ui_Sound_Click));
                    } break;
                    case ui_Click_Released: {
                        view.next_view = to_fornications();

                    } break;
                    default: break;
                }

                CLAY_AUTO_ID({
                    .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
                });

                switch (ui_small_button(
                    ui_icon(ui_Icon_BackToMap),
                    save.run.food < map_cost
                )) {
                    case ui_Click_Pressed: {
                        RL_PlaySound(ui_sound(ui_Sound_CampLeave));
                    } break;
                    case ui_Click_Released: {
                        view.next_view = (view_Transition) {
                            .kind = view_TransitionKind_BackToWorldMap
                        };
                    } break;
                    default: break;
                }
            }
        }
    }
    return Clay_EndLayout(RL_GetFrameTime());
}
