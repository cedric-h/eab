#include "raylib.h"
#include "ui.h"
#include "view.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

static struct {
    View view;
} game = {
    .view = View_Title,
};

void frame(void) {
    View next_view = View_NONE;

#define VIEW_UPDATE(update) \
    next_view = update; \
    if (next_view) { \
        game.view = next_view; \
        next_view = View_NONE; \
        goto start; \
    }

start:

    switch (game.view) {
        case View_NONE:
        case View_Title: {
            VIEW_UPDATE(view_title_update());
            view_title_render();
        }; break;
    }

#undef VIEW_UPDATE
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

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(frame, 60, 1);
#else
    RL_SetTargetFPS(60);
    while (!RL_WindowShouldClose())
        frame();
#endif

    ui_free();
    RL_CloseAudioDevice();
    RL_CloseWindow();

    return 0;
}
