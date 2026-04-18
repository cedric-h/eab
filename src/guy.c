#include "raylib.h"
#include "base.h"
#include "save.h"
#include "guy.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

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
Color guy_color_skin(guy_Guy *guy) {
    return color_lerp(
        color_lerp(
            guy->genes[guy_GeneLoc_SkinColor1]->color,
            guy->genes[guy_GeneLoc_SkinColor2]->color,
            0.5
        ),
        color_lerp(
            guy->genes[guy_GeneLoc_SkinColor2]->color,
            guy->genes[guy_GeneLoc_SkinColor3]->color,
            0.5
        ),
        0.5
    );
}
Color guy_color_hair(guy_Guy *guy) {
    return color_lerp(
        color_lerp(
            guy->genes[guy_GeneLoc_HairColor1]->color,
            guy->genes[guy_GeneLoc_HairColor2]->color,
            0.5
        ),
        color_lerp(
            guy->genes[guy_GeneLoc_HairColor2]->color,
            guy->genes[guy_GeneLoc_HairColor3]->color,
            0.5
        ),
        0.5
    );
}

// guy_Guy *guy_alloc(void) {
//     for (size_t i = 0; i < countof(
// }

guy_Guy guy_guy_init(guy_Race race, guy_Sex sex) {
    guy_Guy guy = {
        .sex = sex,
        .state = guy_GuyState_Inited,
        .hp = 10,
    };

    for (int loc = 0; loc < guy_GeneLoc_COUNT; loc++) {

        /* find applicable gene for this loc and sex/race */
        float applicable_count = 0;
        for (size_t cfg_i = 0; cfg_i < countof(guy_gene_configs); cfg_i++) {
            guy_GeneConfig *cfg = guy_gene_configs + cfg_i;

            if ((cfg->sex & sex) &&
                (cfg->race == race) &&
                (cfg->category == guy_gene_loc_categories[loc])
            ) {
                applicable_count += 1;
            }
        }

        size_t applicable_idx = floorf(RL_GetRandomValue(0, applicable_count - 1));
        guy_GeneConfig *applicable = NULL;
        for (size_t cfg_i = 0; cfg_i < countof(guy_gene_configs); cfg_i++) {
            guy_GeneConfig *cfg = guy_gene_configs + cfg_i;

            if ((cfg->sex & sex) &&
                (cfg->race == race) &&
                (cfg->category == guy_gene_loc_categories[loc])
            ) {
                if (applicable_idx == 0) {
                    applicable = cfg;
                    break;
                }
                applicable_idx -= 1;
            }
        }

        if (applicable) {
            guy.genes[loc] = applicable;
        }
    }

    return guy;
}

static struct {
    RL_Texture body, body_bg, sword;
    RL_Texture assets[guy_Asset_COUNT];
} guy;

static RL_Texture load_and_premultiply(char *path) {
    RL_Image i = RL_LoadImage(path);
    RL_ImageAlphaPremultiply(&i);

    RL_Texture t = RL_LoadTextureFromImage(i);
    RL_SetTextureFilter(t, TEXTURE_FILTER_BILINEAR);
    RL_UnloadImage(i);
    return t;
}

void guy_init() {
    guy.body = load_and_premultiply("./resources/guy/body.png");
    guy.body_bg = load_and_premultiply("./resources/guy/body_bg.png");
    guy.sword = load_and_premultiply("./resources/guy/sword.png");

    for (int i = guy_Asset_NONE+1; i < guy_Asset_COUNT; i++)
        guy.assets[i] = load_and_premultiply(guy_asset_paths[i]);
}
void guy_free() {
    RL_UnloadTexture(guy.body);
    RL_UnloadTexture(guy.body_bg);
    RL_UnloadTexture(guy.sword);
    for (int i = guy_Asset_NONE+1; i < guy_Asset_COUNT; i++)
        RL_UnloadTexture(guy.assets[i]);
}

void guy_draw(guy_Guy *guy_guy, float x, float y) {
    float size = 40*guy_size(guy_guy);
    Color skin = color_lerp(
        guy_color_skin(guy_guy),
        (Color) { 255, 255, 255, 255 },
        0.02
    );
    Color skin_outline = color_lerp(
        guy_color_skin(guy_guy),
        (Color) { 0, 0, 0, 255 },
        0.02
    );
    Color hair = guy_color_hair(guy_guy);


    RL_BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    RL_DrawTexturePro(
        guy.body_bg,
        (RL_Rectangle) { 0, 0, guy.body_bg.width, guy.body_bg.height },
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
        (RL_Color) { skin_outline.r, skin_outline.g, skin_outline.b, skin_outline.a }
    );

    {
        float hair_size = size + 20;
        RL_Texture t = guy.assets[guy_guy->genes[guy_GeneLoc_HairAsset]->asset];
        RL_DrawTexturePro(
            t,
            (RL_Rectangle) { 0, 0, t.width, t.height },
            (RL_Rectangle) {
                x - hair_size*0.65,
                y - hair_size/2,
                hair_size,
                hair_size
            },
            (RL_Vector2) { 0, 0 },
            0,
            (RL_Color) { hair.r, hair.g, hair.b, hair.a }
        );
    }

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

    RL_EndBlendMode();

    {
        ui_Font font = ui_Font_Name;
        char name[20] = {0};
        snprintf(
            name,
            20,
            "%s%s%s",
            guy_guy->genes[guy_GeneLoc_NamePrefix]->str,
            guy_guy->genes[guy_GeneLoc_NameBase]->str,
            guy_guy->genes[guy_GeneLoc_NameSuffix]->str
        );
        float w = RL_MeasureTextEx(ui_font_rl(font), name, ui_font_size(font), 1).x;
        RL_DrawTextEx(
            ui_font_rl(font),
            name,
            (RL_Vector2) { x - w/2, y + size*0.6 },
            ui_font_size(font),
            1,
            (RL_Color) { 0, 0, 0, 255 }
        );
    }
}

// guy_Guy guy_breed(guy_Guy *mom, guy_Guy *Dad) {
//     assert(mom->sex == guy_Sex_Female);
//     assert(dad->sex == guy_Sex_Male);
// 
//     guy_Sex sex = (RL_GetRandomValue() < 0.5f) ? guy_Sex_Male : guy_Sex_Female;
//     guy_Guy kid = {
//         .sex = sex,
//         .state = guy_GuyState_Inited,
//         .hp = 10,
//     };
// 
//     for (size_t i = 0; i < guy_GeneLoc_COUNT; i++) {
//         guy_GeneConfig *mom_gene = mom.genes[i];
//         guy_GeneConfig *dad_gene = dad.genes[i];
// 
//         if ((mom_gene & kid.sex) && (dad_gene & kid.sex))
//             kid.genes[i] = (RL_GetRandomValue() < 0.5f) ? mom_gene : dad_gene;
//         else {
//             if (sex == guy_Sex_Male)
//                 kid.genes[i] = dad_gene;
//             else
//                 kid.genes[i] = mom_gene;
//         }
//     }
// 
//     return kid;
// }
