#include "raylib.h"
#include "view.h"
#include "save.h"
#include "guy.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

static struct {
    view_Transition next_view;
    struct { guy_Guy *mom, *dad, *kid; } families[countof(save.run.guys)];
} view;

void view_fornications_init(view_Transition t) {
    memset(&view, 0, sizeof(view));

    size_t male_count = 0;
    for (size_t i = 0; i < countof(t.fornications.in_orgy); i++) {
        guy_Guy *dad = t.fornications.in_orgy[i];
        if (dad == NULL || !(dad->sex & guy_Sex_Male)) continue;
        male_count += 1;
    }
    assert(male_count > 0);

    size_t kid_i = 0;
    for (size_t i = 0; i < countof(t.fornications.in_orgy); i++) {
        guy_Guy *mom = t.fornications.in_orgy[i];
        if (mom == NULL || !(mom->sex & guy_Sex_Female)) continue;

        size_t dad_idx = roundf(RL_GetRandomValue(0, male_count - 1));

        for (size_t j = 0; j < countof(t.fornications.in_orgy); j++) {
            guy_Guy *dad = t.fornications.in_orgy[j];
            if (dad == NULL || !(dad->sex & guy_Sex_Male)) continue;

            if (dad_idx == 0) {
                guy_Guy *kid = guy_alloc();
                if (kid == NULL) {
                    printf("no space for kid!");
                    break;
                }

                *kid = guy_from_parents(mom, dad);
                view.families[kid_i].mom = mom;
                view.families[kid_i].dad = dad;
                view.families[kid_i].kid = kid;
                kid_i++;

                break;
            }

            dad_idx -= 1;
        }
    }
}
void view_fornications_free(void) {}

view_Transition view_fornications_update(uint64_t _) {
    ui_update();
    return view.next_view;
}
static Clay_RenderCommandArray ui_create_layout(void);
void view_fornications_render(void) {
    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);
    ui_render(ui_create_layout());
    RL_EndDrawing();
}

static void ui_tally(guy_Guy *mom, guy_Guy *dad, guy_Guy *kid) {
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
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } }
        });

        CLAY_TEXT(CLAY_STRING("+"), ui_font(ui_Font_Button));
        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } }
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
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } }
        });

        CLAY_TEXT(CLAY_STRING("="), ui_font(ui_Font_Button));

        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } }
        });

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(45),
                    .height = CLAY_SIZING_FIXED(45),
                },
            },
            .custom = { .customData = kid }
        });

        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } }
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

        CLAY(CLAY_ID("tally_holder"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_GROW(0)
                },
                .childGap = 32,
            },
            .backgroundColor = {0}
        }) {
            for (size_t i = 0; i < countof(view.families); i++) {
                if (view.families[i].mom == NULL ||
                    view.families[i].dad == NULL)
                    continue;
                ui_tally(view.families[i].mom,
                         view.families[i].dad,
                         view.families[i].kid);
            }
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
            CLAY_STRING("BACK"),
            ui_icon(ui_Icon_Camp)
        )) {
            case ui_Click_Pressed: RL_PlaySound(ui_sound(ui_Sound_PageTurn)); break;
            case ui_Click_Released: {
                view.next_view.kind = view_TransitionKind_BackToCampFromFornications;
            } break;
            default: break;
        }

    }
    return Clay_EndLayout(RL_GetFrameTime());
}
