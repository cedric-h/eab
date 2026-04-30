#include "save.h"

save_Save save = {};

int save_count_furniture(save_Furniture furniture) {
    int ret = 0;
    for (size_t i = 0; i < countof(save.run.furniture); i++) {
        if (save.run.furniture[i] == save_Furniture_NONE)
            continue;

        ret += save.run.furniture[i] == furniture;
    }

    return ret;
}

save_FurnitureConfig save_furniture_configs[] = {
    [save_Furniture_Tikitorch] = {
        ui_Icon_Fire,
        30,
        STRING(
            "Rain fire from above"
        )
    },
    [save_Furniture_PocketCamp] = {
        ui_Icon_Bed,
        40,
        STRING(
            "Camp on any tile\n"
        )
    },
    [save_Furniture_CrusaderScrolls] = {
        ui_Icon_Scroll,
        50,
        STRING(
            "if all share ancestor:\n"
            "+30% damage\n"
            "otherwise, -30% damage"
        )
    },
    [save_Furniture_Crown] = {
        ui_Icon_Crown,
        30,
        STRING(
            "Crown a unit\n"
            "to x2 all stats\n"
        )
    },
};
_Static_assert(
    countof(save_furniture_configs) == save_Furniture_COUNT,
    "missing furniture config"
);
