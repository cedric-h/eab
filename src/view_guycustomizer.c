#include "raylib.h"
#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

typedef enum {
    gcz_GuySourceKind_Chaos,
    gcz_GuySourceKind_HalfBreed,
    gcz_GuySourceKind_Pure,
    gcz_GuySourceKind_COUNT,
} gcz_GuySourceKind;
static char *gcz_guy_source_names[gcz_GuySourceKind_COUNT] = {
    [gcz_GuySourceKind_Chaos]     = "chaos",
    [gcz_GuySourceKind_HalfBreed] = "half breed",
    [gcz_GuySourceKind_Pure]      = "pure",
};
typedef struct {
    gcz_GuySourceKind kind;
    guy_Sex sex;
    guy_Race races[2];
} gcz_GuySource;

static guy_Guy gcz_guy_from_source(gcz_GuySource source) {
    switch (source.kind) {
        case gcz_GuySourceKind_Chaos: {
            return guy_from_chaos(source.sex);
        } break;
        case gcz_GuySourceKind_HalfBreed: {
            guy_Guy mom = guy_from_race(source.races[0], guy_Sex_Female);
            guy_Guy dad = guy_from_race(source.races[1], guy_Sex_Male);
            return guy_from_parents(&mom, &dad);
        } break;
        case gcz_GuySourceKind_Pure: {
            return guy_from_race(source.races[0], source.sex);
        } break;

        case gcz_GuySourceKind_COUNT: {
        } break;
    }
    return (guy_Guy){0};
}

static struct {
    view_Transition next_view;

    gcz_GuySource source;
} view;

void view_guycustomizer_init(view_Transition _) {
    memset(&view, 0, sizeof(view));

    view.source.kind = gcz_GuySourceKind_HalfBreed;
    view.source.races[0] = guy_Race_Moai;
    view.source.races[1] = guy_Race_Bunny;
    view.source.sex = guy_Sex_Female;
    save.run.guys[0] = gcz_guy_from_source(view.source);
    // ui_guy_show_detail_page(&save.run.guys[0]);
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


static void gcz_race_input(guy_Race *out) {

    CLAY_AUTO_ID({
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .childGap = 8,
            .sizing.height = CLAY_SIZING_FIT(100, 200),
        },
        .clip = {
            .horizontal = true,
            .vertical = true,
            .childOffset = Clay_GetScrollOffset()
        },
    }) {
        for (guy_Race r = 1; r < guy_Race_COUNT; r++) {
            char *race_name = guy_race_names[r];

            CLAY_AUTO_ID({ .layout.padding.left = 16 }) {
                Clay_String tmp;
                ui_sprintf(
                    tmp,
                    "[%s] %s%s",
                    (r == *out) ? "x" : "  ",
                    race_name,
                    Clay_Hovered() ? " <-" : ""
                );

                CLAY_TEXT(tmp, ui_font(ui_Font_Desc));

                if (Clay_Hovered() && (
                    Clay_GetPointerState().state == 
                        CLAY_POINTER_DATA_RELEASED_THIS_FRAME
                ))
                    *out = r;
            }
        }
    }
}

static void gcz_sex_input(guy_Sex *sex_out) {
    CLAY_AUTO_ID({
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .childGap = 8,
        }
    }) {
        for (int i = 0; i < 2; i++) {
            guy_Sex sex = i ? guy_Sex_Male : guy_Sex_Female;
            char *sex_name = i ? "male" : "female";

            CLAY_AUTO_ID({ .layout.padding.left = 16 }) {
                Clay_String tmp;
                ui_sprintf(
                    tmp,
                    "[%s] %s%s",
                    (sex == *sex_out) ? "x" : "  ",
                    sex_name,
                    Clay_Hovered() ? " <-" : ""
                );

                CLAY_TEXT(tmp, ui_font(ui_Font_Desc));

                if (Clay_Hovered() && (
                    Clay_GetPointerState().state == 
                        CLAY_POINTER_DATA_RELEASED_THIS_FRAME
                ))
                    *sex_out = sex;
            }
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

        CLAY_TEXT(CLAY_STRING("guy tester"), ui_font(ui_Font_SubTitle));
        CLAY_TEXT(CLAY_STRING(" "), ui_font(ui_Font_Button));
        CLAY_TEXT(CLAY_STRING("method"), ui_font(ui_Font_Desc));

        CLAY_AUTO_ID({
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childGap = 8,
            }
        }) {
            for (
                gcz_GuySourceKind sauce = 0;
                sauce < gcz_GuySourceKind_COUNT;
                sauce++
            ) {
                char *sauce_name = gcz_guy_source_names[sauce];

                CLAY_AUTO_ID({ .layout.padding.left = 16 }) {
                    Clay_String tmp;
                    ui_sprintf(
                        tmp,
                        "[%s] %s%s",
                        (sauce == view.source.kind) ? "x" : "  ",
                        sauce_name,
                        Clay_Hovered() ? " <-" : ""
                    );

                    CLAY_TEXT(tmp, ui_font(ui_Font_Desc));

                    if (Clay_Hovered() && (
                        Clay_GetPointerState().state == 
                            CLAY_POINTER_DATA_RELEASED_THIS_FRAME
                    ))
                        view.source.kind = sauce;
                }
            }
        }

        CLAY_TEXT(CLAY_STRING(" "), ui_font(ui_Font_Button));

        switch (view.source.kind) {

            case gcz_GuySourceKind_COUNT:
            case gcz_GuySourceKind_Chaos: {
                CLAY_TEXT(CLAY_STRING("sex"), ui_font(ui_Font_Desc));
                gcz_sex_input(&view.source.sex);
            } break;

            case gcz_GuySourceKind_HalfBreed: {
                CLAY_TEXT(CLAY_STRING("mom race"), ui_font(ui_Font_Desc));
                gcz_race_input(&view.source.races[0]);

                CLAY_TEXT(CLAY_STRING(" "), ui_font(ui_Font_Button));

                CLAY_TEXT(CLAY_STRING("dad race"), ui_font(ui_Font_Desc));
                gcz_race_input(&view.source.races[1]);
            } break;

            case gcz_GuySourceKind_Pure: {
                CLAY_TEXT(CLAY_STRING("race"), ui_font(ui_Font_Desc));
                gcz_race_input(&view.source.races[0]);

                CLAY_TEXT(CLAY_STRING(" "), ui_font(ui_Font_Button));

                CLAY_TEXT(CLAY_STRING("sex"), ui_font(ui_Font_Desc));
                gcz_sex_input(&view.source.sex);
            } break;

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
                save.run.guys[0] = gcz_guy_from_source(view.source);
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

