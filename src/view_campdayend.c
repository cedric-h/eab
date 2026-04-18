#include "raylib.h"
#include "view.h"
#include "save.h"
#include "guy.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    view_Transition next_view;
} view;

void view_campdayend_init(view_Transition _) {
    memset(&view, 0, sizeof(view));
}
void view_campdayend_free(void) {}

view_Transition view_campdayend_update(void) {
    ui_update();
    return view.next_view;
}
static Clay_RenderCommandArray ui_create_layout(void);
void view_campdayend_render(void) {
    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);
    ui_render(ui_create_layout());
    RL_EndDrawing();
}

static void ui_tally(guy_Guy *mom, guy_Guy *dad) {
    CLAY_AUTO_ID({
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW()
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
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
            .custom = { .customData = mom }
        });

        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(15) } }
        });

        CLAY_TEXT(CLAY_STRING("+"), ui_font(ui_Font_Button));
        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(15) } }
        });

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(45),
                    .height = CLAY_SIZING_FIXED(45),
                },
            },
            .custom = { .customData = dad }
        });

        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(15) } }
        });

        CLAY_TEXT(CLAY_STRING("="), ui_font(ui_Font_Button));

        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(15) } }
        });

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(45),
                    .height = CLAY_SIZING_FIXED(45),
                },
            },
        });

        CLAY_TEXT(CLAY_STRING("!"), ui_font(ui_Font_Button));

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
                CLAY_STRING("fornications!"),
                ui_font(ui_Font_Button),
            );
        }

        ui_tally(&save.run.guys[0], &save.run.guys[1]);

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
            CLAY_STRING("BACK"),
            ui_icon(ui_Icon_Camp)
        )) {
            case ui_Click_Pressed: RL_PlaySound(ui_sound(ui_Sound_PageTurn)); break;
            case ui_Click_Released: {
                view.next_view.kind = view_TransitionKind_BackToCampFromDayEnd;
            } break;
            default: break;
        }

    }
    return Clay_EndLayout(RL_GetFrameTime());
}
