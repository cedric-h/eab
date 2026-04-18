/* guy.h draws guys */

#ifndef __EAB_GUY_IMPL
#define __EAB_GUY_IMPL

typedef enum {
    guy_GeneCategory_NONE,
    guy_GeneCategory_SkinColor,
    guy_GeneCategory_Size,
} guy_GeneCategory;

typedef enum {
    guy_GeneLoc_NONE,
    guy_GeneLoc_Color1,
    guy_GeneLoc_Color2,
    guy_GeneLoc_Color3,
    guy_GeneLoc_Color4,
    guy_GeneLoc_ColorLast = guy_GeneLoc_Color4,

    guy_GeneLoc_Size1,
    guy_GeneLoc_Size2,
    guy_GeneLoc_Size3,
    guy_GeneLoc_Size4,
    guy_GeneLoc_SizeLast = guy_GeneLoc_Size4,

    guy_GeneLoc_COUNT,
} guy_GeneLoc;

static guy_GeneCategory guy_gene_loc_categories[] = {
    [guy_GeneLoc_Color1] = guy_GeneCategory_SkinColor,
    [guy_GeneLoc_Color2] = guy_GeneCategory_SkinColor,
    [guy_GeneLoc_Color3] = guy_GeneCategory_SkinColor,
    [guy_GeneLoc_Color4] = guy_GeneCategory_SkinColor,

    [guy_GeneLoc_Size1] = guy_GeneCategory_Size,
    [guy_GeneLoc_Size2] = guy_GeneCategory_Size,
    [guy_GeneLoc_Size3] = guy_GeneCategory_Size,
    [guy_GeneLoc_Size4] = guy_GeneCategory_Size,
};
_Static_assert(
    countof(guy_gene_loc_categories) == guy_GeneLoc_COUNT,
    "missing gene category for gene loc"
);

typedef enum {
    guy_Race_Human,
    guy_Race_Goblin,
    guy_Race_Elf,
} guy_Race;

typedef enum {
    guy_Sex_Male   = (1 << 0),
    guy_Sex_Female = (1 << 1),
    guy_Sex_Any    = guy_Sex_Male | guy_Sex_Female,
} guy_Sex;

typedef struct {
    guy_GeneCategory category;
    guy_Race race;
    guy_Sex sex;
    union {
        Color color;
        float size;
    };
} guy_GeneConfig;

static guy_GeneConfig guy_gene_configs[] = {
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Male  , .size = 1+.100 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Male  , .size = 1+.050 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Male  , .size = 1+.020 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1+.015 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1+.010 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1-.010 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1-.015 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1-.050 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Female, .size = 1-.100 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Female, .size = 1-.125 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Female, .size = 1-.150 },

    // { 63, 192, 87, 255 } goblin color

    {
        .category = guy_GeneCategory_SkinColor,
        .race = guy_Race_Human,
        .sex = guy_Sex_Any,
        .color = { 244, 214, 201, 255 }
    },
    {
        .category = guy_GeneCategory_SkinColor,
        .race = guy_Race_Human,
        .sex = guy_Sex_Any,
        .color = { 238, 193, 143, 255 }
    },
    {
        .category = guy_GeneCategory_SkinColor,
        .race = guy_Race_Human,
        .sex = guy_Sex_Any,
        .color = { 248, 213, 162, 255 }
    },
    {
        .category = guy_GeneCategory_SkinColor,
        .race = guy_Race_Human,
        .sex = guy_Sex_Any,
        .color = { 251, 228, 199, 255 }
    },
    {
        .category = guy_GeneCategory_SkinColor,
        .race = guy_Race_Human,
        .sex = guy_Sex_Any,
        .color = { 134, 89, 66, 255 }
    },
    {
        .category = guy_GeneCategory_SkinColor,
        .race = guy_Race_Human,
        .sex = guy_Sex_Any,
        .color = { 72, 37, 18, 255 }
    },
    {
        .category = guy_GeneCategory_SkinColor,
        .race = guy_Race_Human,
        .sex = guy_Sex_Any,
        .color = { 45, 33, 35, 255 }
    },
    {
        .category = guy_GeneCategory_SkinColor,
        .race = guy_Race_Human,
        .sex = guy_Sex_Any,
        .color = { 245, 189, 162, 255 }
    },
};

typedef enum {
    guy_GuyState_NONE,
    guy_GuyState_Inited,
} guy_GuyState;
typedef struct {
    guy_GuyState state;
    guy_GeneConfig *genes[guy_GeneLoc_COUNT];
    uint16_t hp;
} guy_Guy;

uint16_t guy_maxhp(guy_Guy *guy);
float guy_size(guy_Guy *guy);
Color guy_color(guy_Guy *guy);

void guy_init();
void guy_free();
void guy_draw(guy_Guy *guy, float x, float y);
#endif
