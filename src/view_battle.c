#include "raylib.h"
#include "view.h"
#include "ui.h"
#include "save.h"
#include <string.h>
#include <stdio.h>

typedef struct {
    guy_Guy *guy;
    double swing_t;
    f2 pos;
} battle_Guy;

#define BADDIE_MAX_COUNT 60
static struct {
    view_Transition next_view;
    guy_Guy baddies[BADDIE_MAX_COUNT];
    battle_Guy guys[countof(save.run.guys) + BADDIE_MAX_COUNT];
} view;

void view_battle_init(view_Transition _) {
    memset(&view, 0, sizeof(view));

    float w = RL_GetScreenWidth();
    float h = RL_GetScreenHeight();
    size_t guy_i = 0;

    /* breed up some baddies */
    {
        guy_Guy mom = guy_from_race(guy_Race_Human, guy_Sex_Female);
        guy_Guy dad = guy_from_race(guy_Race_Human, guy_Sex_Male);
        for (int i = 0; i < BADDIE_MAX_COUNT; i++) {
            view.baddies[i] = guy_from_parents(&mom, &dad);

            float x = RL_GetRandomValue(w*0.1, w*0.9);
            float y = RL_GetRandomValue(h*0.1, h*0.3);
            view.guys[guy_i++] = (battle_Guy) {
                .guy = view.baddies + i,
                .pos = { x, y },
            };

        }
    }

    for (size_t i = 0; i < countof(save.run.guys); i++) {
        guy_Guy *guy = save.run.guys + i;
        if (guy->state == guy_GuyState_NONE) continue;

        float x = RL_GetRandomValue(w*0.1, w*0.9);
        float y = RL_GetRandomValue(h*0.7, h*0.9);
        view.guys[guy_i++] = (battle_Guy) {
            .guy = guy,
            .pos = { x, y },
        };
    }
}
void view_battle_free(void) {}

static float battle_guy_size(battle_Guy *bg) {
    return 30.0f * guy_size(bg->guy);
}

view_Transition view_battle_update(void) {
    ui_update();

    if (RL_IsMouseButtonPressed(0)) {
        for (size_t bguy_i = 0; bguy_i < countof(view.guys); bguy_i++) {
            battle_Guy *bguy = view.guys + bguy_i;
            if (bguy->guy == NULL) continue;
            if (bguy_i %2) bguy->swing_t = RL_GetTime();
        }
    }

    /* push guys out of each other */
    for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
        battle_Guy *i = view.guys + guy_i;
        if (i->guy == NULL) continue;

        for (size_t guy_j = guy_i+1; guy_j < countof(view.guys); guy_j++) {
            battle_Guy *j = view.guys + guy_j;
            if (j->guy == NULL) continue;

            float dx = j->pos.x - i->pos.x;
            float dy = j->pos.y - i->pos.y;
            float dist = sqrtf(dx*dx + dy*dy);
            float overlap = (battle_guy_size(i) + battle_guy_size(j)) - dist;
            if (overlap > 0) {
                j->pos.x += (dx/dist) * overlap/2;
                j->pos.y += (dy/dist) * overlap/2;
                i->pos.x -= (dx/dist) * overlap/2;
                i->pos.y -= (dy/dist) * overlap/2;
            }
        }
    }

    return view.next_view;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_battle_render(void) {
    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);

    for (size_t i = 0; i < countof(view.guys); i++) {
        battle_Guy *bguy = view.guys + i;
        if (bguy->guy == NULL) continue;

        f2 m = { RL_GetMousePosition().x, RL_GetMousePosition().y };
        guy_draw_ex(bguy->guy, bguy->pos, m, bguy->swing_t, 0);
    }

    // ui_render(ui_create_layout());
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
