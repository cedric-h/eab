#include "raylib.h"
#include "view.h"
#include "ui.h"
#include "save.h"
#include <string.h>
#include <stdio.h>

static struct {
    view_Transition next_view;
    struct {
        guy_Guy *guy;
        struct { float x, y; } pos;
    } guys[countof(save.run.guys)];
} view;

void view_battle_init(view_Transition _) {
    memset(&view, 0, sizeof(view));

    for (size_t i = 0; i < countof(save.run.guys); i++) {
        guy_Guy *guy = save.run.guys + i;
        if (guy->state == guy_GuyState_NONE) continue;

        float w = RL_GetScreenWidth();
        float h = RL_GetScreenHeight();
        float x = RL_GetRandomValue(w*0.1, w*0.9);
        float y = RL_GetRandomValue(h*0.1, h*0.9);

        view.guys[i].guy = guy;
        view.guys[i].pos.x = x;
        view.guys[i].pos.y = y;
    }
}
void view_battle_free(void) {}

view_Transition view_battle_update(void) {
    ui_update();
    return view.next_view;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_battle_render(void) {
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
            .padding = { 32, 32, 32, 32 },
            .childGap = 16,
        },
        .backgroundColor = {0}
    }) {

        switch (ui_big_button(
            CLAY_STRING("victory"),
            ui_icon(ui_Icon_Crown)
        )) {
            case ui_Click_Pressed: {
                RL_PlaySound(ui_sound(ui_Sound_BattleVictory));
            } break;
            case ui_Click_Released: {
                view.next_view = (view_Transition) {
                    .kind = view_TransitionKind_BattleVictory,
                    .battle_victory = {
                        .coin = 23,
                        .food = 13,
                    }
                };
            } break;
            default: break;
        }

        switch (ui_big_button(
            CLAY_STRING("defeat"),
            ui_icon(ui_Icon_Scroll)
        )) {
            case ui_Click_Pressed: {
                RL_PlaySound(ui_sound(ui_Sound_BattleDefeat));
            } break;
            case ui_Click_Released: {
                memset(&save, 0, sizeof(save));
                view.next_view.kind = view_TransitionKind_BattleDefeat;
            } break;
            default: break;
        }
    }

    return Clay_EndLayout(RL_GetFrameTime());
}
