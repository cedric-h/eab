#include "raylib.h"
#include "base.h"
#include "save.h"
#include "guy.h"
#include "ease.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>

/* raw meta stats */
static float guy_girth(guy_Guy *guy) {
    float x = 0;

    size_t n = 0;
    for (size_t i = guy_GeneLoc_Girth1; i <= guy_GeneLoc_GirthLast; i++)
        n++, x += guy->genes[i]->amount;
    return x/(float)n;
}
static float guy_strength(guy_Guy *guy) {
    float x = 0;

    size_t n = 0;
    for (size_t i = guy_GeneLoc_Strength1; i <= guy_GeneLoc_StrengthLast; i++)
        n++, x += guy->genes[i]->amount;
    return x/(float)n;
}
static float guy_metabolism(guy_Guy *guy) {
    return guy->genes[guy_GeneLoc_Metabolism]->amount;
}
float guy_fecundity(guy_Guy *guy) {
    return guy->genes[guy_GeneLoc_Fecundity]->amount;
}

/* girth: +size, +max hp, -knockback received, -movement speed, +hunger, +meat */
/* strength: +damage, +knockback dealt, +movement speed */
/* metabolism: +movement speed, +hunger, +attack speed, +heals, -longevity */
uint16_t guy_maxhp(guy_Guy *g) {
    return roundf(guy_girth(g) * 100.0f);
}
float guy_speed(guy_Guy *g) {
    return 2*(guy_metabolism(g) + guy_strength(g)*0.2 - guy_girth(g)*0.2);
}
uint32_t guy_initiative(guy_Guy *g) {
    float speed = 100.0f / guy_metabolism(g);
    return roundf(gaussian_randf(speed, speed*0.2));
}
float guy_size(guy_Guy *g) {
    return guy_girth(g);
}
float guy_damage(guy_Guy *g) {
    return guy_strength(g)*15;
}
float guy_meat(guy_Guy *g) {
    return guy_girth(g);
}
float guy_hunger(guy_Guy *g) {
    return guy_metabolism(g) + guy_girth(g)*0.2;
}
uint32_t guy_childcount(guy_Guy *g) {
    float f = guy_fecundity(g);
    float o = roundf(gaussian_randf(f, f*0.32f));
    return max(1, o);
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

guy_Guy *guy_alloc(void) {
    for (size_t i = 0; i < countof(save.run.guys); i++) {
        if (save.run.guys[i].state == guy_GuyState_NONE)
            return save.run.guys + i;
    }
    return NULL;
}

guy_Guy guy_from_race(guy_Race race, guy_Sex sex) {
    guy_Guy guy = {
        .sex = sex,
        .state = guy_GuyState_Inited,
    };

    for (guy_GeneLoc loc = 1; loc < guy_GeneLoc_COUNT; loc++) {

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

        if (applicable_count == 0) {
            raise(SIGTRAP);
        }

        size_t applicable_idx = RL_GetRandomValue(0, applicable_count - 1);
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

    guy.hp = guy_maxhp(&guy);
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

void guy_system_init() {
    guy.body = load_and_premultiply("./resources/guy/body.png");
    guy.body_bg = load_and_premultiply("./resources/guy/body_bg.png");
    guy.sword = load_and_premultiply("./resources/guy/sword.png");

    for (int i = guy_Asset_NONE+1; i < guy_Asset_COUNT; i++)
        guy.assets[i] = load_and_premultiply(guy_asset_paths[i]);
}
void guy_system_free() {
    RL_UnloadTexture(guy.body);
    RL_UnloadTexture(guy.body_bg);
    RL_UnloadTexture(guy.sword);
    for (int i = guy_Asset_NONE+1; i < guy_Asset_COUNT; i++)
        RL_UnloadTexture(guy.assets[i]);
}

void guy_name(guy_Guy *guy_guy, char name[GUY_NAME_LEN_MAX]) {
    snprintf(
        name,
        GUY_NAME_LEN_MAX,
        "%s%s%s",
        guy_guy->genes[guy_GeneLoc_NamePrefix]->str,
        guy_guy->genes[guy_GeneLoc_NameBase]->str,
        guy_guy->genes[guy_GeneLoc_NameSuffix]->str
    );
}

void guy_draw(guy_Guy *guy_guy, float x, float y, guy_DrawFlags flags) {
    guy_draw_ex(
        guy_guy,
        (f2) { x, y },
        (f2) { x, y },
        0,
        0,
        flags
    );
}
void guy_draw_ex(
    guy_Guy *guy_guy,
    f2 pos,
    f2 target,
    double swing_t,
    double hurt_t,
    guy_DrawFlags flags
) {
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

    if (hurt_t > 0 && (RL_GetTime() - hurt_t) < 0.4) {
        double t = ease_out_circ((RL_GetTime() - hurt_t) / 0.4);
        skin        .r = lerp(min(255,         skin.r + 32),         skin.r, t);
        skin_outline.r = lerp(min(255, skin_outline.r + 32), skin_outline.r, t);
        hair        .r = lerp(min(255, hair        .r + 32), hair        .r, t);
    }

    RL_BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    RL_DrawTexturePro(
        guy.body_bg,
        (RL_Rectangle) { 0, 0, guy.body_bg.width, guy.body_bg.height },
        (RL_Rectangle) {
            pos.x - size/2,
            pos.y - size/2,
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
            pos.x - size/2,
            pos.y - size/2,
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
                pos.x - hair_size*0.65,
                pos.y - hair_size/2,
                hair_size,
                hair_size
            },
            (RL_Vector2) { 0, 0 },
            0,
            (RL_Color) { hair.r, hair.g, hair.b, hair.a }
        );
    }

    {
        float sword_size = size * 0.8 * guy_strength(guy_guy);

        /* from the origin to the pommel */
        float pommel_x = sword_size*0.2;
        float pommel_y = sword_size*0.6;

        float sword_x = pos.x + 10 + pommel_x;
        float sword_y = pos.y + pommel_y;
        size_t i = (size_t)(void *)guy_guy%69;

        float rot = sinf(GOLDEN_RATIO*i + RL_GetTime()*2) * 5;
        do {
            if (!(flags & guy_DrawFlags_Target))
                continue;

            float anim_speed = 1.2f;
            float dx = pos.x - target.x;
            float dy = pos.y - target.y;
            if ((fabsf(dx) + fabsf(dy)) == 0)
                continue;

            rot = rot / 180.0f * M_PI;
            rot += atan2f(dy, dx);
            rot -= M_PI*0.75;
            if (swing_t != 0) {
                double t = (RL_GetTime() - swing_t) * anim_speed * 2;

                float rot_start = rot;
                float rad_overshoot     = rot - M_PI*0.50;
                float rad_from          = rot - M_PI*0.40;
                float rad_to            = rot + M_PI*0.40;
                float rad_followthrough = rot + M_PI*0.55;

                double prepare_t = t / 0.1;
                if (prepare_t > 0 && prepare_t < 1) {
                    rot = lerp_rads(rot_start, rad_from, ease_in_back(prepare_t));
                }
                t -= 0.1;

                double hold_t = t / 0.3;
                if (hold_t > 0 && hold_t < 1) {
                    rot = lerp_rads(
                        rad_from,
                        rad_overshoot,
                        hold_t
                    );
                }
                t -= 0.3;

                double attack_t = t / 0.35;
                if (attack_t > 0 && attack_t < 1) {
                    rot = lerp_rads(rad_overshoot, rad_to, ease_in_back(attack_t));
                }
                t -= 0.35;

                double followthrough_t = t / 0.15;
                if (followthrough_t > 0 && followthrough_t < 1) {
                    rot = lerp_rads(
                        rad_to,
                        rad_followthrough,
                        followthrough_t
                    );
                }
                t -= 0.15;

                double return_t = t / 0.3;
                if (return_t > 0 && return_t < 1) {
                    rot = lerp_rads(
                        rad_followthrough,
                        rot_start,
                        ease_in_back(return_t)
                    );
                }
                t -= 0.3;
            }

            if (swing_t != 0) {
                float dl = sqrtf(dx*dx + dy*dy);

                /* push sword_x towards rot */
                double t = (RL_GetTime() - swing_t)*anim_speed*0.8;
                float fwd_travel = 35;
                float back_travel = -8;

                double back_t = t / 0.1;
                if (back_t < 1) {
                    float a = lerpf(0, back_travel, ease_in_back(back_t));
                    sword_x -= dx/dl * a;
                    sword_y -= dy/dl * a;
                }
                t -= 0.1;

                double stab_t = t / 0.1;
                if (stab_t > 0 && stab_t < 1) {
                    float a = lerpf(back_travel, fwd_travel, ease_in_back(stab_t));
                    sword_x -= dx/dl * a;
                    sword_y -= dy/dl * a;
                }
                t -= 0.1;

                double return_t = t / 0.3;
                if (return_t > 0 && return_t < 1) {
                    sword_x -= dx/dl * lerpf(fwd_travel, 0, return_t);
                    sword_y -= dy/dl * lerpf(fwd_travel, 0, return_t);
                }
                t -= 0.3;
            }

            rot = (rot / M_PI) * 180;
        } while (false);

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
            rot,
            RL_BLACK
        );
    }

    RL_EndBlendMode();

    if (flags & guy_DrawFlags_Hp && guy_guy->hp != guy_maxhp(guy_guy)) {
        float w = 40;

        float t = (float)guy_guy->hp / (float)guy_maxhp(guy_guy);
        w *= t;

        RL_Color good = { 100, 255, 100, 255 };
        RL_Color mid  = { 255, 255, 100, 255 };
        RL_Color bad  = { 255, 100, 100, 255 };
        RL_Color clr = (t > 0.5) ?
            ColorLerp(good, mid, inv_lerpf(1.0f, 0.5f, t)) :
            ColorLerp( mid, bad, inv_lerpf(0.5f, 0.0f, t));

        RL_DrawRectangle(
            pos.x - w/2,
            pos.y + size*0.6,
            w,
            5,
            (RL_Color) { clr.r, clr.g, clr.b, 155 }
        );
    }

    if (flags & guy_DrawFlags_Name) {
        ui_Font font = ui_Font_Name;
        char name[GUY_NAME_LEN_MAX] = {0};
        guy_name(guy_guy, name);
        float w = RL_MeasureTextEx(ui_font_rl(font), name, ui_font_size(font), 1).x;
        RL_DrawTextEx(
            ui_font_rl(font),
            name,
            (RL_Vector2) { pos.x - w/2, pos.y + size*0.6 },
            ui_font_size(font),
            1,
            (RL_Color) { 0, 0, 0, 255 }
        );
    }
}

guy_Guy guy_from_parents(guy_Guy *mom, guy_Guy *dad) {
    assert(mom->sex == guy_Sex_Female);
    assert(dad->sex == guy_Sex_Male);

    guy_Sex sex = (RL_GetRandomValue(0, 1) < 0.5f) ? guy_Sex_Male : guy_Sex_Female;
    guy_Guy kid = {
        .sex = sex,
        .state = guy_GuyState_Inited,
        .hp = 10,
    };

    for (guy_GeneLoc i = 1; i < guy_GeneLoc_COUNT; i++) {
        guy_GeneConfig *mom_gene = mom->genes[i];
        guy_GeneConfig *dad_gene = dad->genes[i];

        assert(mom_gene != NULL);
        assert(dad_gene != NULL);

        if ((mom_gene->sex & kid.sex) && (dad_gene->sex & kid.sex))
            kid.genes[i] = (RL_GetRandomValue(0, 1) < 0.5f) ? mom_gene : dad_gene;
        else {
            if (sex == guy_Sex_Male)
                kid.genes[i] = dad_gene;
            else
                kid.genes[i] = mom_gene;
        }
    }

    kid.hp = guy_maxhp(&kid);
    return kid;
}
