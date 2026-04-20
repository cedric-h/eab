#include "raylib.h"
#include "view.h"
#include "ui.h"
#include "save.h"
#include "draw.h"
#include <string.h>
#include <stdio.h>

typedef enum {
    battle_Team_Player,
    battle_Team_Baddie,
    battle_Team_COUNT,
} battle_Team;
typedef enum {
    battle_GuyPhase_NONE,
    battle_GuyPhase_Approaching,
    battle_GuyPhase_Attacking,
    battle_GuyPhase_Hurting,
} battle_GuyPhase;

typedef struct battle_Guy battle_Guy;
struct battle_Guy {
    /* if this is 0, you are said to be "out of phase," a euphemism for "dead" */
    battle_GuyPhase phase;
    battle_Team team;

    guy_Guy *guy;
    battle_Guy *target;

    /* used for hop animation */
    uint64_t last_attack_update;
    float distance_traveled_last_update;
    double swing_t;

    /* initiative exists to solve the problem of
     * "this guy hit me first, and he just kept hitting me until I died."
     * (this is a problem because you never get a swing in edgewise)
     *
     * at initialization and after an attack is launched, initiative is reset to
     * an integer like 10. 
     * whenever the enemy is in range, one is subtracted from the initiative.
     *
     * when it reaches 0, an attack is launched and initiative is reset.
     **/
    uint32_t initiative;

    f2 pos;

    struct {
        double t;
        f2 knockback;
    } last_hurt;
};

#define BADDIE_MAX_COUNT 6
#define COMBATANT_MAX_COUNT (countof(save.run.guys) + BADDIE_MAX_COUNT)
static struct {
    view_Transition next_view;
    guy_Guy baddies[BADDIE_MAX_COUNT];

    battle_Guy guys[COMBATANT_MAX_COUNT];
    struct { bool active; f2 pos; } graves[COMBATANT_MAX_COUNT];
} view;

static uint32_t battle_guy_initiative(battle_Guy *_) {
    return 10;
}

static battle_Guy battle_guy_init(battle_Guy g) {
    g.initiative = battle_guy_initiative(&g);
    return g;
}

void view_battle_init(view_Transition _) {
    memset(&view, 0, sizeof(view));

    float w = RL_GetScreenWidth();
    float h = RL_GetScreenHeight();
    size_t guy_i = 0;

    /* breed up some baddies */
    {
        guy_Guy mom = guy_from_race(guy_Race_Human, guy_Sex_Female);
        guy_Guy dad = guy_from_race(guy_Race_Human, guy_Sex_Male);
        for (int i = 0; i < BADDIE_MAX_COUNT; i++) {
            view.baddies[i] = guy_from_parents(&mom, &dad);

            float x = lerp(w*0.1, w*0.9, (float)i / (float)(BADDIE_MAX_COUNT - 1));
            float y = RL_GetRandomValue(h*0.1, h*0.3);
            view.guys[guy_i++] = battle_guy_init((battle_Guy) {
                .phase = battle_GuyPhase_Approaching,
                .team = battle_Team_Baddie,
                .guy = view.baddies + i,
                .pos = { x, y },
            });
        }
    }

    size_t guy_count = 0;
    for (size_t i = 0; i < countof(save.run.guys); i++)
        guy_count += save.run.guys[i].state != guy_GuyState_NONE;

    for (size_t i = 0; i < countof(save.run.guys); i++) {
        guy_Guy *guy = save.run.guys + i;
        if (guy->state == guy_GuyState_NONE) continue;

        float x = lerp(w*0.1, w*0.9, (float)i / (float)(guy_count - 1));
        float y = RL_GetRandomValue(h*0.7, h*0.9);
        view.guys[guy_i++] = battle_guy_init((battle_Guy) {
            .phase = battle_GuyPhase_Approaching,
            .team = battle_Team_Player,
            .guy = guy,
            .pos = { x, y },
        });
    }
}
void view_battle_free(void) {}

static float battle_guy_size(battle_Guy *bg) {
    return 30.0f * guy_size(bg->guy);
}

view_Transition view_battle_update(uint64_t update) {
    ui_update();

    if (RL_IsMouseButtonPressed(0)) {
        for (size_t bguy_i = 0; bguy_i < countof(view.guys); bguy_i++) {
            battle_Guy *bguy = view.guys + bguy_i;
            if (bguy->phase == battle_GuyPhase_NONE) continue;
            bguy->swing_t = RL_GetTime();
        }
    }

    /* targeting */
    {
        uint16_t guy_target_counts[countof(view.guys)] = {0};

        for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
            battle_Guy *bguy = view.guys + guy_i;
            if (!bguy->phase) continue;

            /* clear outdated targets */
            if (bguy->target != NULL && !bguy->target->phase) {
                bguy->target = NULL;
            }

            if (bguy->target != NULL) {
                size_t target_i = bguy->target - view.guys;
                guy_target_counts[target_i] += 1;
            }
        }

        for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
            battle_Guy *bguy = view.guys + guy_i;
            if (!bguy->phase) continue;

            battle_Guy *current_target = NULL;
            float current_target_score = -999e9;
            for (size_t guy_j = 0; guy_j < countof(view.guys); guy_j++) {
                battle_Guy *target = view.guys + guy_j;
                if (!target->phase) continue;

                /* can't target team members */
                if (target->team == bguy->team) continue;

                float target_score = 0;

                /* distance negatively impacts score */
                /* (probably not worth chasing that really far away guy) */
                float dist = sqrtf(
                    (target->pos.x - bguy->pos.x)*(target->pos.x - bguy->pos.x) +
                    (target->pos.y - bguy->pos.y)*(target->pos.y - bguy->pos.y)
                );
                target_score -= 0.005*dist;
                /* if they're super close - okay maybe just do it */
                if (dist < 40.0f) target_score += 4;

                /* many targets already = lower score */
                /* (anti dog-piling measure) */
                target_score -= guy_target_counts[target - view.guys];

                /* if someone is targeting you, well ... */
                target_score += 2*(target->target == bguy);

                /* prioritize staying locked on to current target */
                target_score += 4*(target == bguy->target);

                if (target_score > current_target_score) {
                    current_target_score = target_score;
                    current_target = target;
                }
            }

            if (bguy->target != current_target) {
                char my_name[GUY_NAME_LEN_MAX] = {0};
                guy_name(bguy->guy, my_name);

                char ex_name[GUY_NAME_LEN_MAX] = {0};
                if (bguy->target) guy_name(bguy->target->guy, ex_name);

                char nu_name[GUY_NAME_LEN_MAX] = {0};
                guy_name(current_target->guy, nu_name);

                printf(
                    "[%s] switching target \"%s\" -> \"%s\"\n",
                    my_name, ex_name, nu_name
                );
            }

            bguy->target = current_target;
            if (current_target) {
                guy_target_counts[current_target - view.guys] += 1;
            }
        }
    }

    /* enemy AI phase state machine */
    {
        for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
            battle_Guy *bguy = view.guys + guy_i;
            float bguy_start_x = bguy->pos.x;
            float bguy_start_y = bguy->pos.y;

            /* distance towards which the guy moves in the approaching phase */
            float best_dist = 60.0f;
            /* distance within which an blow will land */
            float attack_dist = 70.0f;

            switch (bguy->phase) {
                case battle_GuyPhase_NONE: continue;

                case battle_GuyPhase_Approaching: {
                    if (bguy->target == NULL) break;

                    float dx = bguy->target->pos.x - bguy->pos.x;
                    float dy = bguy->target->pos.y - bguy->pos.y;
                    float dlen = sqrtf(dx*dx + dy*dy);

                    float from_best_dist = dlen - best_dist;
                    {
                        float speed = min(2.0f, fabsf(from_best_dist));
                        bguy->pos.x += dx/dlen * speed * sign(from_best_dist);
                        bguy->pos.y += dy/dlen * speed * sign(from_best_dist);
                    }

                    if (from_best_dist < 5) {

                        if (bguy->initiative == 0) {
                            bguy->phase = battle_GuyPhase_Attacking;
                            bguy->swing_t = RL_GetTime();
                            bguy->last_attack_update = update;
                        } else {
                            bguy->initiative -= 1;
                        }
                    }
                }; break;

                case battle_GuyPhase_Attacking: {
                    if (bguy->target == NULL) {
                        bguy->phase = battle_GuyPhase_Approaching;
                        break;
                    }

                    float dx = bguy->target->pos.x - bguy->pos.x;
                    float dy = bguy->target->pos.y - bguy->pos.y;
                    float dlen = sqrtf(dx*dx + dy*dy);

                    /* "best dist" is a bit closer in attacking phase */
                    float from_best_dist = dlen - best_dist*0.5;
                    {
                        float speed = min(2.0f, fabsf(from_best_dist));
                        bguy->pos.x += dx/dlen * speed * sign(from_best_dist);
                        bguy->pos.y += dy/dlen * speed * sign(from_best_dist);
                    }

                    uint64_t updates_since = update - bguy->last_attack_update;

                    if (updates_since == 20) {
                        if (dlen < attack_dist) {
                            bguy->target->phase = battle_GuyPhase_Hurting;
                            bguy->target->last_hurt.knockback.x += dx/dlen * 15;
                            bguy->target->last_hurt.knockback.y += dy/dlen * 15;
                        }

                        /* initiative isn't spent until the attack is launched */
                        bguy->initiative = battle_guy_initiative(bguy);
                    }

                    /**
                     * you could just have a higher initiative instead of this
                     * pause, but it's probably beneficial to have the guy not
                     * move for a big before launching the next bout */
                    if (updates_since > 30) {
                        bguy->phase = battle_GuyPhase_Approaching;
                    }
                }; break;

                case battle_GuyPhase_Hurting: {
                    bguy->pos.x += bguy->last_hurt.knockback.x;
                    bguy->pos.y += bguy->last_hurt.knockback.y;

                    bguy->last_hurt.knockback.x *= 0.9;
                    bguy->last_hurt.knockback.y *= 0.9;
                    bguy->last_hurt.t = RL_GetTime();

                    float dlen = sqrtf(
                        bguy->last_hurt.knockback.x*bguy->last_hurt.knockback.x +
                        bguy->last_hurt.knockback.y*bguy->last_hurt.knockback.y
                    );
                    if (dlen < 5) {
                        bguy->guy->hp--;
                        if (bguy->guy->hp == 0) {
                            bguy->guy = NULL;
                            bguy->phase = battle_GuyPhase_NONE;

                            for (size_t i = 0; i < countof(view.graves); i++) {
                                if (view.graves[i].active)
                                    continue;
                                view.graves[i].active = true;
                                view.graves[i].pos = bguy->pos;
                                break;
                            }
                        }
                    }
                    if (dlen < 0.1) {
                        bguy->phase = battle_GuyPhase_Approaching;
                    }
                }; break;
            }

            bguy->distance_traveled_last_update = sqrtf(
                (bguy->pos.x - bguy_start_x)*(bguy->pos.x - bguy_start_x) +
                (bguy->pos.y - bguy_start_y)*(bguy->pos.y - bguy_start_y)
            );

        }
    }

    /* push guys out of each other */
    for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
        battle_Guy *i = view.guys + guy_i;
        if (!i->phase) continue;

        for (size_t guy_j = guy_i+1; guy_j < countof(view.guys); guy_j++) {
            battle_Guy *j = view.guys + guy_j;
            if (!j->phase) continue;

            float dx = j->pos.x - i->pos.x;
            float dy = j->pos.y - i->pos.y;
            float dist = sqrtf(dx*dx + dy*dy);
            float overlap = (battle_guy_size(i) + battle_guy_size(j)) - dist;
            if (overlap > 0) {
                j->pos.x += (dx/dist) * overlap/2;
                j->pos.y += (dy/dist) * overlap/2;
                i->pos.x -= (dx/dist) * overlap/2;
                i->pos.y -= (dy/dist) * overlap/2;
            }
        }
    }

    /* keep them on the screen */
    for (size_t guy_j = 0; guy_j < countof(view.guys); guy_j++) {
        battle_Guy *j = view.guys + guy_j;
        if (!j->phase) continue;

        float w = RL_GetScreenWidth();
        float h = RL_GetScreenHeight();
        j->pos.x = max(w*0.1, min(w*0.9, j->pos.x));
        j->pos.y = max(h*0.1, min(h*0.9, j->pos.y));
    }


    return view.next_view;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_battle_render(void) {
    RL_BeginDrawing();
    RL_ClearBackground(RL_WHITE);

    for (size_t i = 0; i < countof(view.graves); i++) {
        if (!view.graves[i].active) continue;
        puts("bruh");
        draw_icon(
            ui_Icon_Grave,
            (draw_Rect) {
                .min_x = view.graves[i].pos.x - 20,
                .min_y = view.graves[i].pos.y - 20,
                .max_x = view.graves[i].pos.x + 20,
                .max_y = view.graves[i].pos.y + 20,
            },
            (Color) { 255, 255, 255, 255 }
        );
    }

    for (size_t i = 0; i < countof(view.guys); i++) {
        battle_Guy *bguy = view.guys + i;
        if (!bguy->phase) continue;

        float x = bguy->pos.x;
        float y = bguy->pos.y;

        y += sinf(RL_GetTime()*20 + GOLDEN_RATIO*i) *
            2 * bguy->distance_traveled_last_update;

        f2 target = bguy->pos;
        if (bguy->target != NULL) {
            target.x = bguy->target->pos.x;
            target.y = bguy->target->pos.y;
        }
        guy_DrawFlags flags = guy_DrawFlags_Hp;
        guy_draw_ex(
            bguy->guy,
            (f2) { x, y },
            target,
            bguy->swing_t,
            bguy->last_hurt.t,
            flags
        );
    }

    // ui_render(ui_create_layout());
    RL_EndDrawing();
}

static Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0)
            },
            .padding = { 32, 32, 32, 32 },
            .childGap = 16,
        },
        .backgroundColor = {0}
    }) {

        switch (ui_big_button(
            CLAY_STRING("victory"),
            ui_icon(ui_Icon_Crown)
        )) {
            case ui_Click_Pressed: {
                RL_PlaySound(ui_sound(ui_Sound_BattleVictory));
            } break;
            case ui_Click_Released: {
                view.next_view = (view_Transition) {
                    .kind = view_TransitionKind_BattleVictory,
                    .battle_victory = {
                        .coin = 23,
                        .food = 13,
                    }
                };
            } break;
            default: break;
        }

        switch (ui_big_button(
            CLAY_STRING("defeat"),
            ui_icon(ui_Icon_Scroll)
        )) {
            case ui_Click_Pressed: {
                RL_PlaySound(ui_sound(ui_Sound_BattleDefeat));
            } break;
            case ui_Click_Released: {
                memset(&save, 0, sizeof(save));
                view.next_view.kind = view_TransitionKind_BattleDefeat;
            } break;
            default: break;
        }
    }

    return Clay_EndLayout(RL_GetFrameTime());
}
