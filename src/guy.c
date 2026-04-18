#include "raylib.h"
#include "base.h"
#include "save.h"
#include "guy.h"
#include <math.h>

uint16_t guy_maxhp(guy_Guy *guy) {
    return 10;
}
float guy_size(guy_Guy *guy) {
    return 23;
}


static struct {
    RL_Texture body, sword;
} guy;

void guy_init() {
    guy.body = RL_LoadTexture("./resources/guy/body.png");
    RL_SetTextureFilter(guy.body, TEXTURE_FILTER_BILINEAR);
    guy.sword = RL_LoadTexture("./resources/guy/sword.png");
    RL_SetTextureFilter(guy.sword, TEXTURE_FILTER_BILINEAR);
}
void guy_free() {
    RL_UnloadTexture(guy.body);
    RL_UnloadTexture(guy.sword);
}

void guy_draw(guy_Guy *guy_guy, float x, float y) {
    float size = 40;
    Color skin = { 63, 192, 87, 255 };

    RL_DrawTexturePro(
        guy.body,
        (RL_Rectangle) { 0, 0, guy.body.width, guy.body.height },
        (RL_Rectangle) {
            x - size/2,
            y - size/2,
            size,
            size
        },
        (RL_Vector2) { 0, 0 },
        0,
        (RL_Color) { skin.r, skin.g, skin.b, skin.a }
    );

    {
        float sword_size = size * 0.8;

        /* from the origin to the pommel */
        float pommel_x = sword_size*0.2;
        float pommel_y = sword_size*0.6;

        float sword_x = x + 10 + pommel_x;
        float sword_y = y + pommel_y;
        size_t i = guy_guy - save.run.guys;
        RL_DrawTexturePro(
            guy.sword,
            (RL_Rectangle) { 0, 0, guy.sword.width, guy.sword.height },
            (RL_Rectangle) {
                sword_x - sword_size/2,
                sword_y - sword_size/2,
                sword_size,
                sword_size
            },
            (RL_Vector2) {
                pommel_x,
                pommel_y,
            },
            sinf(i+RL_GetTime()*2)*5,
            RL_BLACK
        );
    }
}
