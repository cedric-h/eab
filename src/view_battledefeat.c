#include "save.h"
#include "raylib.h"
#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    view_Transition next_view;
} view;

void view_battledefeat_init(view_Transition t) {
    memset(&view, 0, sizeof(view));
}
void view_battledefeat_free(void) {}

view_Transition view_battledefeat_update(void) {
    ui_update();
    return view.next_view;
}
static Clay_RenderCommandArray ui_create_layout(void);
void view_battledefeat_render(void) {
    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);
    ui_render(ui_create_layout());
    RL_EndDrawing();
}

static void ui_tally(char *prefix, ui_Icon icon, uint32_t count, char *desc) {
    CLAY_AUTO_ID({
        .layout = {
            .childAlignment = {
                .y = CLAY_ALIGN_Y_CENTER,
            },
        }
    }) {

        {
            Clay_String tmp;
            ui_sprintf(tmp, "%s", prefix);
            CLAY_TEXT(tmp, ui_font(ui_Font_Button));
        }

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
            .image = { .imageData = ui_icon(icon) }
        });

        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(15) } }
        });

        {
            Clay_String tmp;
            ui_sprintf(tmp, "x%d %s", count, desc);
            CLAY_TEXT(tmp, ui_font(ui_Font_Button));
        }
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
                CLAY_STRING("defeat"),
                ui_font(ui_Font_SubTitle),
            );
        }

        CLAY_AUTO_ID({
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .padding = { 0, 0, 32, 32 },
                .childGap = 50,
            },
        }) {
            ui_tally("+", ui_Icon_Shovel, save.run.kills, "kills");
            ui_tally("+", ui_Icon_Fleur, save.run.coin, "unspent");
            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_FIXED(0) } },
            });
            ui_tally("=", ui_Icon_Diamond, save.run.kills, "total");
        }


        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(20),
                    .height = CLAY_SIZING_GROW(),
                },
            },
        });

        switch (ui_big_button(
            CLAY_STRING("DONE"),
            ui_icon(ui_Icon_Swords)
        )) {
            case ui_Click_Pressed: RL_PlaySound(ui_sound(ui_Sound_Click)); break;
            case ui_Click_Released: {
                view.next_view.kind = view_TransitionKind_Title;
            } break;
            default: break;
        }

    }
    return Clay_EndLayout(RL_GetFrameTime());
}
