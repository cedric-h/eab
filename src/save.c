#include "save.h"

save_Save save = {};

save_FurnitureConfig save_furniture_configs[] = {
    [save_Furniture_Tikitorch      ] = {
        ui_Icon_Fire,
        30,
        STRING(
            "Rain fire from above"
        )
    },
    [save_Furniture_Bed            ] = {
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
