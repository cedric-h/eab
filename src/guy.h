/* guy.h draws guys */

#ifndef __EAB_GUY_IMPL
#define __EAB_GUY_IMPL

typedef enum {
    guy_GuyState_NONE,
    guy_GuyState_Inited,
} guy_GuyState;
typedef struct {
    guy_GuyState state;
    uint16_t hp;
} guy_Guy;

uint16_t guy_maxhp(guy_Guy *guy);
float guy_size(guy_Guy *guy);

void guy_init();
void guy_free();
void guy_draw(guy_Guy *guy, float x, float y);
#endif
