#include "raylib.h"
#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    view_Transition next_view;

    guy_Race race;
    guy_Sex sex;
} view;

void view_guycustomizer_init(view_Transition _) {
    memset(&view, 0, sizeof(view));

    view.race = guy_Race_Bunny;
    view.sex = guy_Sex_Female;
    save.run.guys[0] = guy_from_race(view.race, view.sex);
    ui_guy_show_detail_page(&save.run.guys[0]);
}
void view_guycustomizer_free(void) {}

view_Transition view_guycustomizer_update(uint64_t _) {
    ui_update();
    return view.next_view;
}
static Clay_RenderCommandArray ui_create_layout(void);
void view_guycustomizer_render(void) {
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

        CLAY_TEXT(CLAY_STRING("race"), ui_font(ui_Font_Button));
        for (guy_Race r = 1; r < guy_Race_COUNT; r++) {
            char *race_name = guy_race_names[r];

            CLAY_AUTO_ID({ .layout.padding.left = 16 }) {
                Clay_String tmp;
                ui_sprintf(
                    tmp,
                    "[%s] %s%s",
                    (r == view.race) ? "x" : "  ",
                    race_name,
                    Clay_Hovered() ? " <-" : ""
                );

                CLAY_TEXT(tmp, ui_font(ui_Font_Desc));

                if (Clay_Hovered() && (
                    Clay_GetPointerState().state == 
                        CLAY_POINTER_DATA_RELEASED_THIS_FRAME
                ))
                    view.race = r;
            }
        }

        CLAY_TEXT(CLAY_STRING("sex"), ui_font(ui_Font_Button));
        for (int i = 0; i < 2; i++) {
            guy_Sex sex = i ? guy_Sex_Male : guy_Sex_Female;
            char *sex_name = i ? "male" : "female";

            CLAY_AUTO_ID({ .layout.padding.left = 16 }) {
                Clay_String tmp;
                ui_sprintf(
                    tmp,
                    "[%s] %s%s",
                    (sex == view.sex) ? "x" : "  ",
                    sex_name,
                    Clay_Hovered() ? " <-" : ""
                );

                CLAY_TEXT(tmp, ui_font(ui_Font_Desc));

                if (Clay_Hovered() && (
                    Clay_GetPointerState().state == 
                        CLAY_POINTER_DATA_RELEASED_THIS_FRAME
                ))
                    view.sex = sex;
            }
        }

        CLAY_AUTO_ID({
            .border = {
                .width = CLAY_BORDER_OUTSIDE(4),
                .color = {0, 0, 0, 255}
            },
            .backgroundColor = Clay_Hovered()
                ? (Clay_Color) { 128, 128, 128, 128 }
                : (Clay_Color) { 255, 255, 255, 255 },
            .cornerRadius = CLAY_CORNER_RADIUS(6),
            .layout = {
                .padding = { 16, 16, 8, 8 },
            }
        }) {
            if (Clay_Hovered() && (
                Clay_GetPointerState().state == 
                        CLAY_POINTER_DATA_RELEASED_THIS_FRAME
            )) {
                save.run.guys[0] = guy_from_race(view.race, view.sex);
                ui_guy_show_detail_page(&save.run.guys[0]);
            }

            CLAY_TEXT(
                CLAY_STRING("GO"),
                ui_font(ui_Font_Button)
            );
        }


    }
    return Clay_EndLayout(RL_GetFrameTime());
}

