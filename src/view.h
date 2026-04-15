
#define views \
    x(        title, View_Title        ) \
    x(      options, View_Options      ) \
    x(     camptech, View_CampTech     ) \
    x(     worldmap, View_WorldMap     ) \
    x(       battle, View_Battle       ) \
    x(battlevictory, View_BattleVictory) \
    x(    furniture, View_Furniture    ) \
    /*
    x(         camp, View_Camp         ) \
    x( battledefeat, View_BattleDefeat ) \
    */


typedef enum {
    View_NONE,
#define x(_, view_enum) view_enum,
views
#undef x
    View_COUNT,
} View;

#define x(view_name, _) \
    void view_##view_name##_init  (void); \
    void view_##view_name##_free  (void); \
    View view_##view_name##_update(void); \
    void view_##view_name##_render(void);
views
#undef x

#ifdef VIEW_HANDLERS
typedef struct {
    void (*init)(void);
    void (*free)(void);
    View (*update)(void);
    void (*render)(void);
} ViewHandler;

static ViewHandler view_handlers[View_COUNT] = {
#define x(view_name, view_enum) \
    [view_enum] = { \
        .init   = view_##view_name##_init  , \
        .free   = view_##view_name##_free  , \
        .update = view_##view_name##_update, \
        .render = view_##view_name##_render, \
    },
views
#undef x
};
#endif// VIEW_HANDLERS

#undef views
