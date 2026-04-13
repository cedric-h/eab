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

void ui_big_button_click(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    void *userData
) {
    View btn_view = (View)(uint64_t)userData;

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        if (btn_view == View_Title) // View_WorldMap)
            RL_PlaySound(ui_sound(ui_Sound_CinematicOpening));
        else
            RL_PlaySound(ui_sound(ui_Sound_Click));
    }

    if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
        view.next_view = btn_view;
    }
}

void ui_big_button(Clay_String text, RL_Texture *icon, View next_view) {
    CLAY(CLAY_SIDI(text, next_view), {
        .border = {
            .width = CLAY_BORDER_OUTSIDE(4),
            .color = {0, 0, 0, 255}
        },
        .backgroundColor = Clay_Hovered()
            ? (Clay_Color) { 128, 128, 128, 128 }
            : (Clay_Color) { 255, 255, 255, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .layout = {
            .padding = { 32, 32, 24, 16 },
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIT(0),
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        }
    }) {
        Clay_OnHover(ui_big_button_click, (void *)(uint64_t)next_view);

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(60),
                    .height = CLAY_SIZING_FIXED(60),
                },
            },
            .image = { .imageData = icon }
        }) {
        }

        /* spacer */
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(20, 0),
                    .height = CLAY_SIZING_FIXED(20),
                },
            },
        }) {
        }

        CLAY_TEXT(
            text,
            (Clay_TextElementConfig) {
                .fontSize = 45,
                .fontId = ui_Font_Button,
                .textColor = { 0, 0, 0, 255 },
            },
        );
    }
}

Clay_RenderCommandArray ui_create_layout(void) {
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

            ui_big_button(
                CLAY_STRING("to battle!"),
                ui_icon(ui_Icon_Swords),
                View_Title // View_WorldMap
            );

            ui_big_button(
                CLAY_STRING("camp tech"),
                ui_icon(ui_Icon_Diamond),
                View_Title // View_CampTech
            );

            ui_big_button(
                CLAY_STRING("options"),
                ui_icon(ui_Icon_Wrench),
                View_Title // View_Options
            );
        }
    }
    return Clay_EndLayout(RL_GetFrameTime());
}

View view_title_update(void) {
    ui_update();
    return view.next_view;
}
void view_title_render(void) {
    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);
    ui_render(ui_create_layout());
    RL_EndDrawing();
}
