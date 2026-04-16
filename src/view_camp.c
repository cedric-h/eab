#include "raylib.h"
#include "view.h"
#include "ui.h"
#include "save.h"
#include "draw.h"
#include <math.h>
#include <stdio.h>

#include "svg.h"
#include "svg/orgy_circle.h"

#define ORGY_CIRCLE_X    175
#define ORGY_CIRCLE_Y    305
#define ORGY_CIRCLE_SIZE 250

static struct {
    view_Transition next_view;
    int held_furniture_idx;
} view = {};

void view_camp_init(view_Transition t) {
    memset(&view, 0, sizeof(view));

    view.held_furniture_idx = -1;
}
void view_camp_free(void) {
}

view_Transition view_camp_update(void) {

    for (size_t i = 0; i < countof(save.run.furniture); i++) {
        save_CampFurniture *fc = save.run.furniture + i;
        float from_center =
            sqrtf((ORGY_CIRCLE_X - fc->pos.x)*(ORGY_CIRCLE_X - fc->pos.x) +
                  (ORGY_CIRCLE_Y - fc->pos.y)*(ORGY_CIRCLE_Y - fc->pos.y));

        float from_edge = (ORGY_CIRCLE_SIZE/2 - 5) - from_center;
        float from_edge_min = 50;
        if (fabsf(from_edge) < from_edge_min) {
            float out_x = (fc->pos.x - ORGY_CIRCLE_X) / from_center;
            float out_y = (fc->pos.y - ORGY_CIRCLE_Y) / from_center;
            fc->pos.x += out_x * (from_edge - from_edge_min*sign(from_edge))*0.4;
            fc->pos.y += out_y * (from_edge - from_edge_min*sign(from_edge))*0.4;
        }
    }

    ui_update();
    return view.next_view;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_camp_render(void) {
    RL_BeginDrawing();

    RL_ClearBackground(RL_WHITE);

    if (view.held_furniture_idx != -1) {
        save_CampFurniture *f = save.run.furniture + view.held_furniture_idx;
        RL_Vector2 m = RL_GetMousePosition();
        f->pos.x = m.x;
        f->pos.y = m.y;
    }

    if (RL_IsMouseButtonReleased(0))
        view.held_furniture_idx = -1;

    /* draw human recycler */
    {
        float x = 380;
        float y = 150;
        float size = 60;

        ui_Font font = ui_Font_Desc;
        RL_DrawTextEx(
            ui_font_rl(font),
            "human",
            (RL_Vector2) { x - size*0.6, y - size*0.8 },
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

        {
            float icon_size = size;
            float t = (view.held_furniture_idx != -1)*fabsf(sinf(RL_GetTime()*7));

            RL_Vector2 m = RL_GetMousePosition();
            float dist = sqrtf((x - m.x)*(x - m.x) +
                               (y - m.y)*(y - m.y));
            icon_size *= 1 + 0.2*fabsf(min(1, dist - size)) / size;

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

    /* draw furniture */
    for (size_t i = 0; i < countof(save.run.furniture); i++) {
        save_CampFurniture f = save.run.furniture[i];
        save_FurnitureConfig *fc = save_furniture_configs + f.kind;
        if (f.kind == save_Furniture_NONE) continue;

        float size = 50;

        if (view.held_furniture_idx == -1) {
            RL_Vector2 m = RL_GetMousePosition();
            float dist = sqrtf((f.pos.x - m.x)*(f.pos.x - m.x) +
                               (f.pos.y - m.y)*(f.pos.y - m.y));

            if (dist < size/2) {
                eab_mouse_cursor = MOUSE_CURSOR_POINTING_HAND;
                if (RL_IsMouseButtonPressed(0))
                    view.held_furniture_idx = i;
            }
        }

        draw_icon(
            fc->icon,
            (draw_Rect) {
                .min_x = f.pos.x - size/2,
                .min_y = f.pos.y - size/2,
                .max_x = f.pos.x + size/2,
                .max_y = f.pos.y + size/2,
            },
            (Color) { 255, 255, 255, 255 }
        );
    }

    /* draw orgy circle */
    {
        float x = ORGY_CIRCLE_X;
        float y = ORGY_CIRCLE_Y;
        float size = ORGY_CIRCLE_SIZE;
        RL_Vector2 m = RL_GetMousePosition();
        bool hover = sqrtf((x - m.x)*(x - m.x) + (y - m.y)*(y - m.y)) < (size/2);
        hover = hover && view.held_furniture_idx != -1;
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

                /* bed cost */
                CLAY_TEXT(CLAY_STRING("x5"), ui_font(ui_Font_Cost));

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

                /* leaving cost */
                CLAY_TEXT(CLAY_STRING("x5"), ui_font(ui_Font_Cost));
            }

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } },
            }) {

                switch (ui_small_button(
                        ui_icon(ui_Icon_Bed),
                        save.run.food < 5
                    )) {
                    case ui_Click_Pressed: {
                        RL_PlaySound(ui_sound(ui_Sound_Click));
                    } break;
                    case ui_Click_Released: {
                    } break;
                    default: break;
                }

                CLAY_AUTO_ID({
                    .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
                });

                switch (ui_small_button(
                    ui_icon(ui_Icon_BackToMap),
                    save.run.food < 5
                )) {
                    case ui_Click_Pressed: {
                        RL_PlaySound(ui_sound(ui_Sound_Click));
                    } break;
                    case ui_Click_Released: {
                    } break;
                    default: break;
                }
            }
        }
    }
    return Clay_EndLayout(RL_GetFrameTime());
}
