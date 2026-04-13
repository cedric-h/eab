#include "raylib.h"
#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    View next_view;
} view;


void view_title_init(void) {
    memset(&view, 0, sizeof(view));
}
void view_title_free(void) {}

View view_title_update(void) {
    ui_update();
    return view.next_view;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_title_render(void) {
    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);
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
            .padding = { 16, 16, 16, 16 },
            .childGap = 16,
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
        },
        .backgroundColor = {0}
    }) {

        CLAY(CLAY_ID("title"), {
            .layout = {
                .padding = { 0, 0, 0, 24 },
            }
        }) {
            CLAY_TEXT(
                CLAY_STRING("Eugenics Auto Battler"),
                (Clay_TextElementConfig) {
                    .fontSize = 80,
                    .fontId = ui_Font_Title,
                    .textColor = {0, 0, 0, 255}
                },
            );
        }

        CLAY(CLAY_ID("buttons"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_FIT(0),
                },
                .childGap = 16,
            },
        }) {

            switch (ui_big_button(
                CLAY_STRING("to battle!"),
                ui_icon(ui_Icon_Swords)
            )) {
                case ui_Click_Pressed:
                    RL_PlaySound(ui_sound(ui_Sound_CinematicOpening));
                    break;
                case ui_Click_Released: view.next_view = View_WorldMap; break;
                default: break;
            }

            switch (ui_big_button(
                CLAY_STRING("camp tech"),
                ui_icon(ui_Icon_Diamond)
            )) {
                case ui_Click_Pressed: RL_PlaySound(ui_sound(ui_Sound_Click)); break;
                case ui_Click_Released: view.next_view = View_CampTech; break;
                default: break;
            }

            switch (ui_big_button(
                CLAY_STRING("options"),
                ui_icon(ui_Icon_Wrench)
            )) {
                case ui_Click_Pressed: RL_PlaySound(ui_sound(ui_Sound_Click)); break;
                case ui_Click_Released: view.next_view = View_Options; break;
                default: break;
            }
        }
    }
    return Clay_EndLayout(RL_GetFrameTime());
}
