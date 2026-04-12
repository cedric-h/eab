#include "raylib.h"

#include "ui.h"
#include "clay.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void frame(void) {
    ui_update();

    BeginDrawing();
    ClearBackground(WHITE);
    ui_render();
    EndDrawing();
}

int main(void) {

    SetConfigFlags(
        FLAG_VSYNC_HINT |
        FLAG_WINDOW_RESIZABLE |
        FLAG_MSAA_4X_HINT
    );
    InitWindow(
        9*50,
        16*50,
        "eugenics auto battler"
    );

    ui_init();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(frame, 60, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
        frame();
#endif

    ui_free();
    CloseWindow();

    return 0;
}
