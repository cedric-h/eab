#include "raylib.h"
#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    View next_view;
} view;

void view_battlevictory_init(void) {
    memset(&view, 0, sizeof(view));
}
void view_battlevictory_free(void) {}

View view_battlevictory_update(void) {
    ui_update();
    return view.next_view;
}
static Clay_RenderCommandArray ui_create_layout(void);
void view_battlevictory_render(void) {
    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);
    ui_render(ui_create_layout());
    RL_EndDrawing();
}

void ui_tally(ui_Icon icon, uint32_t count) {
    CLAY_AUTO_ID({
        .layout = {
            .childAlignment = {
                .y = CLAY_ALIGN_Y_CENTER,
            },
        }
    }) {

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(45),
                    .height = CLAY_SIZING_FIXED(45),
                },
            },
            .image = { .imageData = ui_icon(icon) }
        });

        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(15) } }
        });

        Clay_String tmp;
        ui_sprintf(tmp, "x%d", count);
        CLAY_TEXT(tmp, ui_font(ui_Font_Button));
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
            }
        }) {
            CLAY_TEXT(
                CLAY_STRING("victory"),
                ui_font(ui_Font_SubTitle),
            );
        }

        CLAY_AUTO_ID({
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .padding = { 32, 32, 32, 32 },
                .childGap = 32,
            },
        }) {
            ui_tally(ui_Icon_Food, 3);
            ui_tally(ui_Icon_Fleur, 15);
        }


        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(20),
                    .height = CLAY_SIZING_GROW(),
                },
            },
        }) {
        }

        switch (ui_big_button(
            CLAY_STRING("DONE"),
            ui_icon(ui_Icon_Fleur)
        )) {
            case ui_Click_Pressed: RL_PlaySound(ui_sound(ui_Sound_Click)); break;
            case ui_Click_Released: view.next_view = View_Furniture; break;
            default: break;
        }

    }
    return Clay_EndLayout(RL_GetFrameTime());
}
