#include <assert.h>
#include "raylib.h"
#include "ui.h"
#include "save.h"
#define VIEW_HANDLERS
#include "view.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

int eab_mouse_cursor;
static struct {
    View view;
} game = {0};

void frame(void) {
    RL_SetMouseCursor(eab_mouse_cursor);
    eab_mouse_cursor = 0;

    view_Transition transition = { 0 };

start:
    transition = view_handlers[game.view].update();
    if (transition.kind != view_TransitionKind_NONE) {
        view_handlers[game.view].free();

        switch (transition.kind) {
            case view_TransitionKind_NONE: assert(false); break;

            case view_TransitionKind_Title: game.view = View_Title; break;
            case view_TransitionKind_Options: game.view = View_Options; break;
            case view_TransitionKind_CampTech: game.view = View_CampTech; break;

            case view_TransitionKind_StartRun: {
                memset(&save.run, 0, sizeof(save.run));
                save.run.coin = 30;
                save.run.food = 30;
                game.view = View_WorldMap;
            } break;

            case view_TransitionKind_StartCamp: game.view = View_Camp; break;
            case view_TransitionKind_StartBattle: game.view = View_Battle; break;

            case view_TransitionKind_BattleDefeat: { 
                game.view = View_BattleDefeat;
            } break;

            case view_TransitionKind_BattleVictory: { 
                game.view = View_BattleVictory;
            } break;

            case view_TransitionKind_BuyFurniture: {
                game.view = View_Furniture;
            } break;

            case view_TransitionKind_BackToWorldMap: {
                game.view = View_WorldMap;
            } break;

        };

        view_handlers[game.view].init(transition);
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
    game.view = View_Camp;
    save.run.food = 5;
    save.run.furniture[0].kind = save_Furniture_Bed;
    save.run.furniture[0].pos.x = 150;
    save.run.furniture[0].pos.y = 150;
    view_handlers[game.view].init((view_Transition) {0});

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
