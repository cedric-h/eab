#include "raylib.h"
#include "ui.h"
#define VIEW_HANDLERS
#include "view.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

static struct {
    View view;
} game = {
    // .view = View_Title,
    .view = View_WorldMap,
};

void frame(void) {
    View next_view = View_NONE;

start:
    next_view = view_handlers[game.view].update();
    if (next_view) {
        view_handlers[game.view].free();
        view_handlers[next_view].init();
        game.view = next_view;
        goto start;
    }

    view_handlers[game.view].render();
}

int main(void) {

    RL_SetConfigFlags(
        FLAG_VSYNC_HINT |
        // FLAG_WINDOW_RESIZABLE |
        FLAG_MSAA_4X_HINT
    );
    RL_InitWindow(
        9*50,
        16*50,
        "eugenics auto battler"
    );

    RL_InitAudioDevice();

    ui_init();
    view_handlers[game.view].init();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(frame, 60, 1);
#else
    RL_SetTargetFPS(60);
    while (!RL_WindowShouldClose())
        frame();
#endif

    view_handlers[game.view].free();
    ui_free();
    RL_CloseAudioDevice();
    RL_CloseWindow();

    return 0;
}
