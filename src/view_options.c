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
                    .fontSize = 60,
                    .fontId = ui_Font_Button,
                    .textColor = {0, 0, 0, 255}
                },
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
