#include "raylib.h"
#include "clay.h"
#include "clay_renderer_raylib.h"

#include "ui.h"

static struct {
    Clay_Arena clay_memory;
    Font fonts[ui_Font_COUNT];
    Clay_RenderCommandArray render_cmds;
} ui = {0};

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
            (float)GetScreenWidth(),
            (float)GetScreenHeight()
        },
        (Clay_ErrorHandler) { ui_handle_clay_errors, 0 }
    );

    Clay_Raylib_Initialize();

    ui.fonts[ui_Font_Body24] = LoadFontEx("resources/Roboto-Regular.ttf", 48, 0, 400);
	SetTextureFilter(ui.fonts[ui_Font_Body24].texture, TEXTURE_FILTER_BILINEAR);
    ui.fonts[ui_Font_Body16] = LoadFontEx("resources/Roboto-Regular.ttf", 32, 0, 400);
    SetTextureFilter(ui.fonts[ui_Font_Body16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, ui.fonts);
}
void ui_free(void) {
    free(ui.clay_memory.memory);

    for (int i = 0; i < ui_Font_COUNT; i++) {
        UnloadFont(ui.fonts[i]);
    }
}

Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();
    return Clay_EndLayout(GetFrameTime());
}

void ui_update(void) {
    Vector2 mouseWheelDelta = GetMouseWheelMoveV();
    float mouseWheelX = mouseWheelDelta.x;
    float mouseWheelY = mouseWheelDelta.y;

    Clay_SetDebugModeEnabled(false);

    Clay_Vector2 mousePosition = {
        GetMousePosition().x,
        GetMousePosition().y,
    };
    Clay_SetPointerState(mousePosition, IsMouseButtonDown(0));
    Clay_SetLayoutDimensions(
        (Clay_Dimensions) {
            (float)GetScreenWidth(),
            (float)GetScreenHeight()
        }
    );

    Clay_UpdateScrollContainers(
        true,
        (Clay_Vector2) {mouseWheelX, mouseWheelY},
        GetFrameTime()
    );
    ui.render_cmds = ui_create_layout();
}

void ui_render(void) {
    Clay_Raylib_Render(ui.render_cmds, ui.fonts);
}
