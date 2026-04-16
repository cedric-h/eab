#ifndef __EAB_VIEW_IMPL
#define __EAB_VIEW_IMPL
#include <stdint.h>

#define views \
    x(        title, View_Title        ) \
    x(      options, View_Options      ) \
    x(     camptech, View_CampTech     ) \
    x(     worldmap, View_WorldMap     ) \
    x(       battle, View_Battle       ) \
    x(battlevictory, View_BattleVictory) \
    x(    furniture, View_Furniture    ) \
    x( battledefeat, View_BattleDefeat ) \
    x(         camp, View_Camp         ) \


typedef enum {
    view_TransitionKind_NONE,
    view_TransitionKind_Title,
    view_TransitionKind_Options,
    view_TransitionKind_CampTech,
    view_TransitionKind_StartRun,
    view_TransitionKind_StartCamp,
    view_TransitionKind_StartBattle,
    view_TransitionKind_BattleVictory,
    view_TransitionKind_BattleDefeat,
    view_TransitionKind_BuyFurniture,
    view_TransitionKind_BackToWorldMap,
} view_TransitionKind;
typedef struct {
    view_TransitionKind kind;
    struct {
        uint32_t food, coin;
    } battle_victory;
} view_Transition;

typedef enum {
    View_NONE,
#define x(_, view_enum) view_enum,
views
#undef x
    View_COUNT,
} View;

#define x(view_name, _) \
    void view_##view_name##_init  (view_Transition); \
    void view_##view_name##_free  (void); \
    view_Transition view_##view_name##_update(void); \
    void view_##view_name##_render(void);
views
#undef x

#ifdef VIEW_HANDLERS
typedef struct {
    void (*init)(view_Transition);
    void (*free)(void);
    view_Transition (*update)(void);
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
#endif
