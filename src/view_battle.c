#include "raylib.h"
#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    View next_view;
} view;


void view_battle_init(void) {
    memset(&view, 0, sizeof(view));
}
void view_battle_free(void) {}

View view_battle_update(void) {
    ui_update();
    return view.next_view;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_battle_render(void) {
    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);
    ui_render(ui_create_layout());
    RL_EndDrawing();
}

static Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();
    return Clay_EndLayout(RL_GetFrameTime());
}
