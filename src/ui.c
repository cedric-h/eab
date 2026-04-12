#include "raylib.h"
#include "clay.h"
#include "clay_renderer_raylib.h"

#include "ui.h"

typedef enum {
    ui_Font_Button,
    ui_Font_Title,
    ui_Font_COUNT,
} ui_Font;

typedef enum {
    ui_Icon_Swords,
    ui_Icon_Diamond,
    ui_Icon_Wrench,
    ui_Icon_COUNT,
} ui_Icon;

char *ui_icon_paths[ui_Icon_COUNT] = {
    [ui_Icon_Swords ] = "resources/icon_swords.png",
    [ui_Icon_Diamond] = "resources/icon_diamond.png",
    [ui_Icon_Wrench ] = "resources/icon_wrench.png",
};

static struct {
    Clay_Arena clay_memory;
    RL_Font fonts[ui_Font_COUNT];
    RL_Texture icons[ui_Icon_COUNT];
    Clay_RenderCommandArray render_cmds;
} ui = {0};

void ui_big_button(Clay_String text, RL_Texture *icon) {
    CLAY_AUTO_ID({
        .border = {
            .width = CLAY_BORDER_OUTSIDE(4),
            .color = {0, 0, 0, 255}
        },
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
                    .width = CLAY_SIZING_GROW(20),
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
                &ui.icons[ui_Icon_Swords]
            );

            ui_big_button(
                CLAY_STRING("camp tech"),
                &ui.icons[ui_Icon_Diamond]
            );

            ui_big_button(
                CLAY_STRING("options"),
                &ui.icons[ui_Icon_Wrench]
            );
        }
    }
    return Clay_EndLayout(RL_GetFrameTime());
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

    const char *font_path = "resources/WackClubSans-Regular.ttf";
    ui.fonts[ui_Font_Button] = RL_LoadFontEx(font_path, 45, 0, 400);
	RL_SetTextureFilter(
        ui.fonts[ui_Font_Button].texture,
        TEXTURE_FILTER_BILINEAR
    );

    ui.fonts[ui_Font_Title] = RL_LoadFontEx(font_path, 80, 0, 400);
    RL_SetTextureFilter(
        ui.fonts[ui_Font_Title].texture,
        TEXTURE_FILTER_BILINEAR
    );

    for (int i = 0; i < ui_Icon_COUNT; i++)
        ui.icons[i] = RL_LoadTexture(ui_icon_paths[i]);

    Clay_SetMeasureTextFunction(Raylib_MeasureText, ui.fonts);
}
void ui_free(void) {
    free(ui.clay_memory.memory);

    for (int i = 0; i < ui_Font_COUNT; i++) {
        RL_UnloadFont(ui.fonts[i]);
    }

    for (int i = 0; i < ui_Icon_COUNT; i++)
        RL_UnloadTexture(ui.icons[i]);
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
    ui.render_cmds = ui_create_layout();
}

void ui_render(void) {
    Clay_Raylib_Render(ui.render_cmds, ui.fonts);
}
