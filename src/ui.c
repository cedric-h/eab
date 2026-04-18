#include "base.h"
#include "view.h"
#include "raylib.h"
#include "clay.h"
#include "clay_renderer_raylib.h"

#include "ui.h"

size_t ui_font_sizes[] = {
    [ui_Font_Button  ] = 35,
    [ui_Font_Title   ] = 60,
    [ui_Font_SubTitle] = 48,
    [ui_Font_Cost    ] = 25,
    [ui_Font_Desc    ] = 18,
    [ui_Font_Name    ] = 10,
};
_Static_assert(countof(ui_font_sizes) == ui_Font_COUNT, "missing font path");

char *ui_icon_paths[] = {
    [ui_Icon_Swords   ] = "resources/icon/swords.png",
    [ui_Icon_Diamond  ] = "resources/icon/diamond.png",
    [ui_Icon_Wrench   ] = "resources/icon/wrench.png",
    [ui_Icon_Back     ] = "resources/icon/back.png",
    [ui_Icon_Forward  ] = "resources/icon/forward.png",
    [ui_Icon_Bed      ] = "resources/icon/bed.png",
    [ui_Icon_Camp     ] = "resources/icon/camp.png",
    [ui_Icon_Fire     ] = "resources/icon/fire.png",
    [ui_Icon_Plant    ] = "resources/icon/plant.png",
    [ui_Icon_Scroll   ] = "resources/icon/scroll.png",
    [ui_Icon_Shovel   ] = "resources/icon/shovel.png",
    [ui_Icon_Crown    ] = "resources/icon/crown.png",
    [ui_Icon_Fleur    ] = "resources/icon/fleur.png",
    [ui_Icon_Food     ] = "resources/icon/food.png",
    [ui_Icon_Dice     ] = "resources/icon/dice.png",
    [ui_Icon_Soup     ] = "resources/icon/soup.png",
    [ui_Icon_BackToMap] = "resources/icon/back_to_map.png",
};
_Static_assert(countof(ui_icon_paths) == ui_Icon_COUNT, "missing icon path");

char *ui_sound_paths[] = {
    [ui_Sound_Click           ] = "resources/audio/click1.wav",
    [ui_Sound_CinematicOpening] = "resources/audio/cinematic_opening.wav",
    [ui_Sound_BattleEnter     ] = "resources/audio/battle_enter.wav",
    [ui_Sound_BattleVictory   ] = "resources/audio/battle_victory.wav",
    [ui_Sound_BattleDefeat    ] = "resources/audio/battle_defeat.wav",
    [ui_Sound_PageTurn        ] = "resources/audio/page_turn.wav",
    [ui_Sound_CampEnter       ] = "resources/audio/camp_enter.wav",
    [ui_Sound_CampLeave       ] = "resources/audio/camp_leave.wav",
};
_Static_assert(countof(ui_sound_paths) == ui_Sound_COUNT, "missing sound path");

static struct {
    Clay_Arena clay_memory;

    char *layout_arena_backing, *layout_arena;
    size_t layout_arena_size;

    RL_Font    fonts [ ui_Font_COUNT];
    RL_Texture icons [ ui_Icon_COUNT];
    RL_Sound   sounds[ui_Sound_COUNT];
} ui = {0};

size_t ui_font_size(ui_Font f) { return ui_font_sizes[f]; }
RL_Font ui_font_rl(ui_Font f) { return ui.fonts[f]; }

Clay_TextElementConfig ui_font(ui_Font f) {
    return ui_font_ex(f, (Clay_TextElementConfig){
        .textColor = { 0, 0, 0, 255 },
    });
}
Clay_TextElementConfig ui_font_ex(ui_Font f, Clay_TextElementConfig tec) {
    tec.fontId = f;
    if (tec.fontSize == 0)
        tec.fontSize = ui_font_sizes[f];
    return tec;
}
RL_Texture *ui_icon(ui_Icon i) { return &ui.icons[i]; }
RL_Sound ui_sound(ui_Sound s) { return ui.sounds[s]; }

Clay_String ui_layout_alloc(size_t alloc_size) {
    size_t new_size = (ui.layout_arena - ui.layout_arena_backing) + alloc_size;
    if (new_size >= ui.layout_arena_size) {
        printf("ran out of space in ui layout arena, resetting it ...\n");
        ui.layout_arena = ui.layout_arena_backing;
    }

    char *base = ui.layout_arena;
    ui.layout_arena += alloc_size;
    return (Clay_String) { .chars = base, .length = alloc_size - 1 };
}

void ui_handle_clay_errors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);

    switch (errorData.errorType) {
        case CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED: {
            Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
            ui_free();
            ui_init();
        } break;

        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED: {
            Clay_SetMaxMeasureTextCacheWordCount(
                Clay_GetMaxMeasureTextCacheWordCount() * 2
            );
            ui_free();
            ui_init();
        } break;

        default:
            break;
    }
}

void ui_init(void) {
    ui.layout_arena_backing = malloc(1 << 12);
    ui.layout_arena_size = 1 << 12;
    ui.layout_arena = ui.layout_arena_backing;

    uint64_t total_mem_size = Clay_MinMemorySize();
    ui.clay_memory = Clay_CreateArenaWithCapacityAndMemory(
        total_mem_size,
        malloc(total_mem_size)
    );
    Clay_Initialize(
        ui.clay_memory,
        (Clay_Dimensions) {
            (float)RL_GetScreenWidth(),
            (float)RL_GetScreenHeight()
        },
        (Clay_ErrorHandler) { ui_handle_clay_errors, 0 }
    );

    Clay_Raylib_Initialize();

    const char *font_path = "resources/WackClubSans-Regular.otf";
    for (int i = 0; i < ui_Font_COUNT; i++) {
        ui.fonts[i] = RL_LoadFontEx(
            font_path,
            ui_font_sizes[i],
            0,
            400
        );
        RL_SetTextureFilter(
            ui.fonts[i].texture,
            TEXTURE_FILTER_BILINEAR
        );
    }

    for (int i = 0; i < ui_Icon_COUNT; i++) {
        ui.icons[i] = RL_LoadTexture(ui_icon_paths[i]);
        RL_SetTextureFilter(
            ui.icons[i],
            TEXTURE_FILTER_BILINEAR
        );
    }

    for (int i = 0; i < ui_Sound_COUNT; i++)
        ui.sounds[i] = RL_LoadSound(ui_sound_paths[i]);

    Clay_SetMeasureTextFunction(Raylib_MeasureText, ui.fonts);
}
void ui_free(void) {
    free(ui.layout_arena_backing);
    free(ui.clay_memory.memory);

    for (int i = 0; i < ui_Font_COUNT; i++)
        RL_UnloadFont(ui.fonts[i]);

    for (int i = 0; i < ui_Icon_COUNT; i++)
        RL_UnloadTexture(ui.icons[i]);

    for (int i = 0; i < ui_Sound_COUNT; i++)
        RL_UnloadSound(ui.sounds[i]);
}

void ui_update(void) {
    RL_Vector2 mouseWheelDelta = RL_GetMouseWheelMoveV();
    float mouseWheelX = mouseWheelDelta.x;
    float mouseWheelY = mouseWheelDelta.y;

    Clay_SetDebugModeEnabled(false);

    Clay_Vector2 mousePosition = {
        RL_GetMousePosition().x,
        RL_GetMousePosition().y,
    };
    Clay_SetPointerState(mousePosition, RL_IsMouseButtonDown(0));
    Clay_SetLayoutDimensions(
        (Clay_Dimensions) {
            (float)RL_GetScreenWidth(),
            (float)RL_GetScreenHeight()
        }
    );

    Clay_UpdateScrollContainers(
        true,
        (Clay_Vector2) {mouseWheelX, mouseWheelY},
        RL_GetFrameTime()
    );
}

void ui_render(Clay_RenderCommandArray render_cmds) {
    Clay_Raylib_Render(render_cmds, ui.fonts);
    ui.layout_arena = ui.layout_arena_backing;
}

ui_Click ui_big_button(Clay_String text, RL_Texture *icon) {
    ui_Click ret = ui_Click_NONE;

    CLAY(CLAY_SIDI(text, icon->id), {
        .border = {
            .width = CLAY_BORDER_OUTSIDE(4),
            .color = {0, 0, 0, 255}
        },
        .backgroundColor = Clay_Hovered()
            ? (Clay_Color) { 128, 128, 128, 128 }
            : (Clay_Color) { 255, 255, 255, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .layout = {
            .padding = { 32, 32, 16, 16 },
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
        if (Clay_Hovered()) {
            eab_mouse_cursor = MOUSE_CURSOR_POINTING_HAND;

            switch (Clay_GetPointerState().state) {
                case CLAY_POINTER_DATA_PRESSED_THIS_FRAME:
                    ret = ui_Click_Pressed;
                    break;
                case CLAY_POINTER_DATA_RELEASED_THIS_FRAME:
                    ret = ui_Click_Released;
                    break;
                default:
                    break;
            }
        }

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(60),
                    .height = CLAY_SIZING_FIXED(60),
                },
            },
            .image = { .imageData = icon }
        });

        /* spacer */
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_FIXED(60),
                },
            },
        });

        CLAY_TEXT(text, ui_font(ui_Font_Button));
    }

    return ret;
}

ui_Click ui_small_button(RL_Texture *icon, bool disabled) {
    ui_Click ret = ui_Click_NONE;

    Clay_Color enabled_black = 
        (disabled)
            ? (Clay_Color) { 120, 120, 120, 255}
            : (Clay_Color) {   0,   0,   0, 255}
        ;

    CLAY(CLAY_IDI("ui_small_button", icon->id), {
        .border = {
            .width = CLAY_BORDER_OUTSIDE(4),
            .color = enabled_black,
        },
        .backgroundColor = (!disabled && Clay_Hovered())
            ? (Clay_Color) { 128, 128, 128, 128 }
            : (Clay_Color) { 255, 255, 255, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .layout = {
            .padding = { 16, 16, 16, 16 },
            .sizing = {
                .width = CLAY_SIZING_FIT(0),
                .height = CLAY_SIZING_FIT(0),
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        }
    }) {
        if (!disabled && Clay_Hovered()) {
            eab_mouse_cursor = MOUSE_CURSOR_POINTING_HAND;

            switch (Clay_GetPointerState().state) {
                case CLAY_POINTER_DATA_PRESSED_THIS_FRAME:
                    ret = ui_Click_Pressed;
                    break;
                case CLAY_POINTER_DATA_RELEASED_THIS_FRAME:
                    ret = ui_Click_Released;
                    break;
                default:
                    break;
            }
        }

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(60),
                    .height = CLAY_SIZING_FIXED(60),
                },
            },
            .image = {
                .imageData = icon,
            },
            .backgroundColor = (disabled)
                ? (Clay_Color) { 255, 255, 255, 200 }
                : (Clay_Color) {   0,   0,   0,   0 },
        });
    }

    return ret;
}

