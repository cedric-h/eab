#include "raylib.h"
#include "ui.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void frame(void) {
    ui_update();

    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);
    ui_render();
    RL_EndDrawing();
}

int main(void) {

    RL_SetConfigFlags(
        FLAG_VSYNC_HINT |
        FLAG_WINDOW_RESIZABLE |
        FLAG_MSAA_4X_HINT
    );
    RL_InitWindow(
        9*50,
        16*50,
        "eugenics auto battler"
    );

    ui_init();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(frame, 60, 1);
#else
    RL_SetTargetFPS(60);
    while (!RL_WindowShouldClose())
        frame();
#endif

    ui_free();
    RL_CloseWindow();

    return 0;
}
