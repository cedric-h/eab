/* guy.h draws guys */

#ifndef __EAB_GUY_IMPL
#define __EAB_GUY_IMPL

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

    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Tomm" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Fredd" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Johnn" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Timm" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Brigg" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Jimm" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Cedd" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Greg" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Samm" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Jamm" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Sham" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Mark" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Nel" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Patt" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Mel" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Nat" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Val" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Shar" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Ray" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jen" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jae" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jay" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Dae" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Tae" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jan" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Ann" },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Any   , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Any   , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Any   , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Any   , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Any   , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Any   , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "or" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "ory" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "ston" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "o" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "o" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "ie" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Male  , .str = "ie" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Female, .str = "a" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Female, .str = "ra" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Female, .str = "na" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase,   .sex = guy_Sex_Female, .str = "ah" },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "rick" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "wise" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "ham" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "han" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "owski" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "son" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "ster" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "man" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "o" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "ie" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "lynn" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "lynne" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "acah" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "lina" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "rina" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "queesha" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "ley" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "ley" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "ah" },

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

guy_Guy guy_guy_init(guy_Race race, guy_Sex sex);

uint16_t guy_maxhp(guy_Guy *guy);
float guy_size(guy_Guy *guy);
Color guy_color_skin(guy_Guy *guy);
Color guy_color_hair(guy_Guy *guy);

void guy_init();
void guy_free();
void guy_draw(guy_Guy *guy, float x, float y);
#endif
