#include "save.h"

save_Save save = {0};

save_FurnitureConfig save_furniture_configs[] = {
    [save_Furniture_Tikitorch      ] = {
        ui_Icon_Fire,
        30,
        STRING(
            "+10% fireball size"
        )
    },
    [save_Furniture_Bed            ] = {
        ui_Icon_Bed,
        40,
        STRING(
            "+40% day-end healing\n"
            "+10% food consumed"
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
};
_Static_assert(
    countof(save_furniture_configs) == save_Furniture_COUNT,
    "missing furniture config"
);
