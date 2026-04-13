typedef enum {
    View_NONE,
    View_Title,
    // View_Options,
    // View_CampTech,
    // View_WorldMap,
    // View_Camp,
    // View_Battle,
    // View_BattleVictory,
    // View_BattleDefeat,
    // View_Furniture,
} View;

void view_title_init(void);
void view_title_free(void);
View view_title_update(void);
void view_title_render(void);

// void view_options_init(void);
// void view_options_free(void);
// View view_options_update(void);
// void view_options_render(void);
