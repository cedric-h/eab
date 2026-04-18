#ifndef __UI_HEADER_IMPL
#define __UI_HEADER_IMPL

#include "raylib.h"
#include "base.h"
#include "clay.h"

typedef enum {
    ui_Font_Button,
    ui_Font_Title,
    ui_Font_SubTitle,
    ui_Font_Cost,
    ui_Font_Desc,
    ui_Font_Name,
    ui_Font_COUNT,
} ui_Font;

typedef enum {
    ui_Icon_Swords,
    ui_Icon_Diamond,
    ui_Icon_Wrench,
    ui_Icon_Back,
    ui_Icon_Forward,
    ui_Icon_Bed,
    ui_Icon_Camp,
    ui_Icon_Fire,
    ui_Icon_Plant,
    ui_Icon_Scroll,
    ui_Icon_Shovel,
    ui_Icon_Crown,
    ui_Icon_Fleur,
    ui_Icon_Food,
    ui_Icon_Dice,
    ui_Icon_Soup,
    ui_Icon_BackToMap,
    ui_Icon_COUNT,
} ui_Icon;

typedef enum {
    ui_Sound_Click,
    ui_Sound_CinematicOpening,
    ui_Sound_BattleEnter,
    ui_Sound_BattleVictory,
    ui_Sound_BattleDefeat,
    ui_Sound_PageTurn,
    ui_Sound_CampEnter,
    ui_Sound_CampLeave,
    ui_Sound_COUNT,
} ui_Sound;

RL_Font ui_font_rl(ui_Font);
size_t ui_font_size(ui_Font);
Clay_TextElementConfig ui_font(ui_Font);
Clay_TextElementConfig ui_font_ex(ui_Font, Clay_TextElementConfig);
RL_Texture *ui_icon(ui_Icon);
RL_Sound ui_sound(ui_Sound);
void ui_init(void);
void ui_free(void);
void ui_update(void);
void ui_render(Clay_RenderCommandArray render_cmds);

Clay_String ui_layout_alloc(size_t size);
#define ui_sprintf(dst, fmt, ...)                                              \
    do {                                                                       \
        int _ui_len = snprintf(NULL, 0, (fmt), ##__VA_ARGS__);                 \
        (dst) = ui_layout_alloc(_ui_len + 1);                                  \
        snprintf((char *)(dst).chars, (dst).length + 1, (fmt), ##__VA_ARGS__); \
    } while (0)

typedef enum {
    ui_Click_NONE,
    ui_Click_Pressed,
    ui_Click_Released,
} ui_Click;

ui_Click ui_big_button(Clay_String text, RL_Texture *icon);
ui_Click ui_small_button(RL_Texture *icon, bool disabled);

#endif
