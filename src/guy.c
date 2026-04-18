#include "raylib.h"
#include "base.h"
#include "save.h"
#include "guy.h"
#include <math.h>
#include <stdio.h>

uint16_t guy_maxhp(guy_Guy *guy) {
    return 10;
}
float guy_size(guy_Guy *guy) {
    float x = 0;

    size_t i = guy_GeneLoc_Size1;
    for (; i <= guy_GeneLoc_SizeLast; i++)
        x += guy->genes[i]->size;
    return x/(i - guy_GeneLoc_Size1);
}
Color guy_color(guy_Guy *guy) {
    return color_lerp(
        color_lerp(
            guy->genes[guy_GeneLoc_Color1]->color,
            guy->genes[guy_GeneLoc_Color2]->color,
            0.5
        ),
        color_lerp(
            guy->genes[guy_GeneLoc_Color2]->color,
            guy->genes[guy_GeneLoc_Color3]->color,
            0.5
        ),
        0.5
    );
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
    float size = 40*guy_size(guy_guy);
    Color skin = guy_color(guy_guy);

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

    {
        ui_Font font = ui_Font_Name;
        char *name = "sharqueesha";
        float w = RL_MeasureTextEx(ui_font_rl(font), name, ui_font_size(font), 1).x;
        RL_DrawTextEx(
            ui_font_rl(font),
            name,
            (RL_Vector2) { x - w/2, y + size*0.8 },
            ui_font_size(font),
            1,
            (RL_Color) { 0, 0, 0, 255 }
        );
    }
}
