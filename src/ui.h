#include "base.h"
#include "clay.h"

typedef enum {
    ui_Font_Button,
    ui_Font_Title,
    ui_Font_COUNT,
} ui_Font;

typedef enum {
    ui_Icon_Swords,
    ui_Icon_Diamond,
    ui_Icon_Wrench,
    ui_Icon_COUNT,
} ui_Icon;

typedef enum {
    ui_Sound_Click,
    ui_Sound_CinematicOpening,
    ui_Sound_COUNT,
} ui_Sound;

RL_Font ui_font(ui_Font);
RL_Texture *ui_icon(ui_Icon);
RL_Sound ui_sound(ui_Sound);
void ui_init(void);
void ui_free(void);
void ui_update(void);
void ui_render(Clay_RenderCommandArray render_cmds);
