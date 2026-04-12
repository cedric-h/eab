#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void frame(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawFPS(10, 10);
    EndDrawing();
}

int main(void) {
    InitWindow(9*50, 16*50, "raylib game template");
    InitAudioDevice();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(frame, 60, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
        frame();
#endif

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
