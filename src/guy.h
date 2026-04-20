/* guy.h draws guys */
#ifndef __EAB_GUY_IMPL
#define __EAB_GUY_IMPL

#include "base.h"

#define guy_assets \
    x(guy_Asset_HairHumanMale1, "./resources/guy/hair_human_male1.png") \
    x(guy_Asset_HairHumanMale2, "./resources/guy/hair_human_male2.png") \
    x(guy_Asset_HairHumanFemale1, "./resources/guy/hair_human_female1.png") \
    x(guy_Asset_HairHumanFemale2, "./resources/guy/hair_human_female2.png")

typedef enum {
#define x(e, _) e,
    guy_Asset_NONE,
guy_assets
#undef x
    guy_Asset_COUNT,
} guy_Asset;

static char *guy_asset_paths[] = {
#define x(e, x) [e] = x,
guy_assets
#undef x
};

#undef guy_assets

typedef enum {
    guy_GeneLoc_NONE,
    guy_GeneLoc_SkinColor1,
    guy_GeneLoc_SkinColor2,
    guy_GeneLoc_SkinColor3,
    guy_GeneLoc_SkinColor4,
    guy_GeneLoc_SkinColorLast = guy_GeneLoc_SkinColor4,

    guy_GeneLoc_HairColor1,
    guy_GeneLoc_HairColor2,
    guy_GeneLoc_HairColor3,
    guy_GeneLoc_HairColor4,
    guy_GeneLoc_HairColorLast = guy_GeneLoc_HairColor4,

    guy_GeneLoc_HairAsset,

    guy_GeneLoc_Size1,
    guy_GeneLoc_Size2,
    guy_GeneLoc_Size3,
    guy_GeneLoc_Size4,
    guy_GeneLoc_SizeLast = guy_GeneLoc_Size4,

    guy_GeneLoc_NamePrefix,
    guy_GeneLoc_NameBase,
    guy_GeneLoc_NameSuffix,

    guy_GeneLoc_COUNT,
} guy_GeneLoc;

typedef enum {
    guy_GeneCategory_NONE,
    guy_GeneCategory_SkinColor,
    guy_GeneCategory_HairColor,
    guy_GeneCategory_HairAsset,
    guy_GeneCategory_Size,
    guy_GeneCategory_NamePrefix,
    guy_GeneCategory_NameBase  ,
    guy_GeneCategory_NameSuffix,
} guy_GeneCategory;

static guy_GeneCategory guy_gene_loc_categories[] = {
    [guy_GeneLoc_SkinColor1] = guy_GeneCategory_SkinColor,
    [guy_GeneLoc_SkinColor2] = guy_GeneCategory_SkinColor,
    [guy_GeneLoc_SkinColor3] = guy_GeneCategory_SkinColor,
    [guy_GeneLoc_SkinColor4] = guy_GeneCategory_SkinColor,

    [guy_GeneLoc_HairColor1] = guy_GeneCategory_HairColor,
    [guy_GeneLoc_HairColor2] = guy_GeneCategory_HairColor,
    [guy_GeneLoc_HairColor3] = guy_GeneCategory_HairColor,
    [guy_GeneLoc_HairColor4] = guy_GeneCategory_HairColor,

    [guy_GeneLoc_HairAsset] = guy_GeneCategory_HairAsset,

    [guy_GeneLoc_Size1] = guy_GeneCategory_Size,
    [guy_GeneLoc_Size2] = guy_GeneCategory_Size,
    [guy_GeneLoc_Size3] = guy_GeneCategory_Size,
    [guy_GeneLoc_Size4] = guy_GeneCategory_Size,

    [guy_GeneLoc_NamePrefix] = guy_GeneCategory_NamePrefix,
    [guy_GeneLoc_NameBase  ] = guy_GeneCategory_NameBase  ,
    [guy_GeneLoc_NameSuffix] = guy_GeneCategory_NameSuffix,
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
        char *str;
        guy_Asset asset;
    };
} guy_GeneConfig;

static guy_GeneConfig guy_gene_configs[] = {
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Male  , .size = 1+0.150 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Male  , .size = 1+0.075 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Male  , .size = 1+0.030 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1+0.022 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1+0.015 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1-0.015 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1-0.022 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Any   , .size = 1-0.075 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Female, .size = 1-0.150 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Female, .size = 1-0.187 },
    { .category = guy_GeneCategory_Size, .race = guy_Race_Human, .sex = guy_Sex_Female, .size = 1-0.225 },

    // { 63, 192, 87, 255 } goblin color

    {
        .category = guy_GeneCategory_HairAsset,
        .race = guy_Race_Human,
        .sex = guy_Sex_Male,
        .asset = guy_Asset_HairHumanMale1,
    },
    {
        .category = guy_GeneCategory_HairAsset,
        .race = guy_Race_Human,
        .sex = guy_Sex_Male,
        .asset = guy_Asset_HairHumanMale2,
    },
    {
        .category = guy_GeneCategory_HairAsset,
        .race = guy_Race_Human,
        .sex = guy_Sex_Female,
        .asset = guy_Asset_HairHumanFemale1,
    },
    {
        .category = guy_GeneCategory_HairAsset,
        .race = guy_Race_Human,
        .sex = guy_Sex_Female,
        .asset = guy_Asset_HairHumanFemale2,
    },


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

    {
        .race = guy_Race_Human,
        .category = guy_GeneCategory_HairColor,
        .sex = guy_Sex_Any,
        .color = { 255, 220, 114, 255 },
    },
    {
        .race = guy_Race_Human,
        .category = guy_GeneCategory_HairColor,
        .sex = guy_Sex_Any,
        .color = { 62, 26, 25, 255 },
    },
    {
        .race = guy_Race_Human,
        .category = guy_GeneCategory_HairColor,
        .sex = guy_Sex_Any,
        .color = { 83, 49, 55, 255 },
    },
    {
        .race = guy_Race_Human,
        .category = guy_GeneCategory_HairColor,
        .sex = guy_Sex_Any,
        .color = { 95, 24, 14, 255 },
    },
    {
        .race = guy_Race_Human,
        .category = guy_GeneCategory_HairColor,
        .sex = guy_Sex_Any,
        .color = { 110, 54, 40, 255 }
    },
    {
        .race = guy_Race_Human,
        .category = guy_GeneCategory_HairColor,
        .sex = guy_Sex_Any,
        .color = { 148, 70, 37, 255 }
    },
    {
        .race = guy_Race_Human,
        .category = guy_GeneCategory_HairColor,
        .sex = guy_Sex_Any,
        .color = { 217, 153, 90, 255 }
    },
    {
        .race = guy_Race_Human,
        .category = guy_GeneCategory_HairColor,
        .sex = guy_Sex_Any,
        .color = { 237, 209, 177, 255 }
    },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Jo " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Mel " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Pat " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Jay " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Tay " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Sal " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Vic " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Nic " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Ash " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Itt " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Archer " },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Kyle " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Tim " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Tom " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Jon " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Jim " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Josh " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Bill " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Joel " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Greg " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Charles " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Karl " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Jeff " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Jack " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Earl " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Guy " },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jill " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Kate " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Cat " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Beth " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Ella " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Elle " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Elli " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jess " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Tina " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Deb " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Debi " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Trish " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jen " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jeni " },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Mc" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "O'" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Brigg" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Den" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Hen" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Ben" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Jen" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Cart" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Sword" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Shield" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Dun" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Run" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Rune" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Dune" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Hill" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Play" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Shakes" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Send" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Man" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Farm" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Kwik" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Ep" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Gold" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Ein" },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "hall" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "ham" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "rourke" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "fawkes" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "things" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "peck" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "stuff" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "croft" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "cross" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "drix" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "son" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "sen" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "mann" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "macher" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "meier" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "stein" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "berg" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "hands" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "malley" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "mason" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "wright" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "mund" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "smith" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "kowski" },
};

typedef enum {
    guy_GuyState_NONE,
    guy_GuyState_Inited,
} guy_GuyState;
typedef struct {
    guy_GuyState state;
    guy_Sex sex;
    guy_GeneConfig *genes[guy_GeneLoc_COUNT];
    uint16_t hp;
} guy_Guy;

guy_Guy guy_from_race(guy_Race race, guy_Sex sex);
guy_Guy guy_from_parents(guy_Guy *mom, guy_Guy *dad);
guy_Guy *guy_alloc(void);

uint16_t guy_maxhp(guy_Guy *guy);
float guy_size(guy_Guy *guy);
float guy_speed(guy_Guy *guy);
uint32_t guy_initiative(guy_Guy *guy);
Color guy_color_skin(guy_Guy *guy);
Color guy_color_hair(guy_Guy *guy);

void guy_system_init();
void guy_system_free();

#define GUY_NAME_LEN_MAX 30
void guy_name(guy_Guy *guy_guy, char name[GUY_NAME_LEN_MAX]);

typedef enum {
    guy_DrawFlags_Name   = (1 << 0),
    guy_DrawFlags_Hp     = (1 << 1),
    guy_DrawFlags_Target = (1 << 2),
} guy_DrawFlags;
void guy_draw(guy_Guy *guy, float x, float y, guy_DrawFlags flags);

void guy_draw_ex(
    guy_Guy *guy,
    f2 pos,
    f2 target,
    double swing_t,
    double hurt_t,
    guy_DrawFlags flags
);
#endif
