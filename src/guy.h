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

    guy_GeneLoc_Strength1,
    guy_GeneLoc_Strength2,
    guy_GeneLoc_Strength3,
    guy_GeneLoc_StrengthLast = guy_GeneLoc_Strength3,

    guy_GeneLoc_Metabolism,
    guy_GeneLoc_Fecundity,

    guy_GeneLoc_Girth1,
    guy_GeneLoc_Girth2,
    guy_GeneLoc_Girth3,
    guy_GeneLoc_Girth4,
    guy_GeneLoc_GirthLast = guy_GeneLoc_Girth4,

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

    guy_GeneCategory_Girth,
    guy_GeneCategory_Strength,
    guy_GeneCategory_Metabolism,
    guy_GeneCategory_Fecundity,

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

    [guy_GeneLoc_Strength1] = guy_GeneCategory_Strength,
    [guy_GeneLoc_Strength2] = guy_GeneCategory_Strength,
    [guy_GeneLoc_Strength3] = guy_GeneCategory_Strength,

    [guy_GeneLoc_Metabolism] = guy_GeneCategory_Metabolism,
    [guy_GeneLoc_Fecundity] = guy_GeneCategory_Fecundity,

    [guy_GeneLoc_Girth1] = guy_GeneCategory_Girth,
    [guy_GeneLoc_Girth2] = guy_GeneCategory_Girth,
    [guy_GeneLoc_Girth3] = guy_GeneCategory_Girth,
    [guy_GeneLoc_Girth4] = guy_GeneCategory_Girth,

    [guy_GeneLoc_NamePrefix] = guy_GeneCategory_NamePrefix,
    [guy_GeneLoc_NameBase  ] = guy_GeneCategory_NameBase  ,
    [guy_GeneLoc_NameSuffix] = guy_GeneCategory_NameSuffix,
};
_Static_assert(
    countof(guy_gene_loc_categories) == guy_GeneLoc_COUNT,
    "missing gene category for gene loc"
);

typedef enum {
    guy_Race_NONE,
    guy_Race_Human,
    guy_Race_Bunny,
    guy_Race_Elf,
    guy_Race_COUNT,
} guy_Race;

static char *guy_race_names[guy_Race_COUNT] = {
    [guy_Race_NONE] = "N/A",
    [guy_Race_Human] = "Human",
    [guy_Race_Bunny] = "Bunny",
    [guy_Race_Elf] = "Elf",
};
_Static_assert(
    countof(guy_race_names) == guy_Race_COUNT,
    "missing name for race"
);

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
        float amount;
        char *str;
        guy_Asset asset;
    };
} guy_GeneConfig;

extern guy_GeneConfig guy_gene_configs[];

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

/* base stats */
float guy_girth(guy_Guy *guy);
float guy_metabolism(guy_Guy *guy);
float guy_fecundity(guy_Guy *guy);
float guy_strength(guy_Guy *guy);

/* derived stats */
uint16_t guy_maxhp(guy_Guy *guy);
float guy_size(guy_Guy *guy);
float guy_speed(guy_Guy *guy);
float guy_damage(guy_Guy *guy);
float guy_meat(guy_Guy *guy); /* r u good eatin? how much meat u got on u?? */
float guy_hunger(guy_Guy *guy);
uint32_t guy_childcount(guy_Guy *guy);
uint32_t guy_initiative(guy_Guy *guy);

Color guy_color_skin(guy_Guy *guy);
Color guy_color_hair(guy_Guy *guy);

void guy_system_init();
void guy_system_free();

#define GUY_NAME_LEN_MAX 30
void guy_name(guy_Guy *guy_guy, char name[GUY_NAME_LEN_MAX]);
char *guy_sex_str(guy_Sex sex);

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
