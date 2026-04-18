/* small, insignificant collection of drawing utils */

#ifndef __EAB_DRAW_IMPL
#define __EAB_DRAW_IMPL

#include "raylib.h"

typedef struct { float min_x, min_y, max_x, max_y; } draw_Rect;
static void draw_icon(ui_Icon icon, draw_Rect rect, Color tint) {
    RL_Texture t = *ui_icon(icon);

    RL_DrawTexturePro(
        t,
        (RL_Rectangle) { 0, 0, t.width, t.height },
        (RL_Rectangle) {
            rect.min_x,
            rect.min_y,
            rect.max_x - rect.min_x,
            rect.max_y - rect.min_y
        },
        (RL_Vector2) { 0, 0 },
        0,
        (RL_Color) { tint.r, tint.g, tint.b, tint.a }
    );
}

#endif
