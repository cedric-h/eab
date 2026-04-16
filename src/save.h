#include "base.h"
#include "ui.h"

typedef enum {
    save_Furniture_NONE,
    save_Furniture_Tikitorch,
    save_Furniture_Bed,
    save_Furniture_CrusaderScrolls,
    save_Furniture_COUNT,
} save_Furniture;

typedef struct {
    ui_Icon icon;
    uint32_t cost;
    String desc;
} save_FurnitureConfig;
extern save_FurnitureConfig save_furniture_configs[];

typedef struct {
    save_Furniture kind;
    struct { float x, y; } pos;
} save_CampFurniture;

typedef struct {
    uint32_t gems;
    
    /* state that gets reset at the end of a run */
    struct {
        uint32_t kills;
        uint32_t food;
        uint32_t coin;
        uint32_t map_progress_idx;
        save_CampFurniture furniture[10];
    } run;

} save_Save;
extern save_Save save;
