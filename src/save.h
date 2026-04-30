#ifndef __EAB_SAVE_IMPL
#define __EAB_SAVE_IMPL
#include "base.h"
#include "guy.h"
#include "ui.h"

typedef enum {
    save_Furniture_NONE,
    save_Furniture_Tikitorch,
    save_Furniture_PocketCamp,
    save_Furniture_Crown,
    save_Furniture_Telescope,
    save_Furniture_CrusaderScrolls,
    save_Furniture_COUNT,
} save_Furniture;

typedef struct {
    ui_Icon icon;
    uint32_t cost;
    /* maximum amount of this type of furniture you can have */
    uint32_t max_count;
    String desc;
} save_FurnitureConfig;
extern save_FurnitureConfig save_furniture_configs[];

int save_count_furniture(save_Furniture);

typedef struct {
    uint32_t gems;
    
    /* state that gets reset at the end of a run */
    struct {
        uint32_t id;

        guy_Guy guys[100];

        uint32_t kills;

        uint32_t key_count;
        float food;
        uint32_t coin;
        save_Furniture furniture[10];
    } run;

} save_Save;
extern save_Save save;
#endif
