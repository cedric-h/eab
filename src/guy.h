/* guy.h draws guys */
#ifndef __EAB_GUY_IMPL
#define __EAB_GUY_IMPL

#include "base.h"

#define guy_assets \
    x(guy_Asset_None, "./resources/guy2/none.png") \
    x(guy_Asset_MoaiFrame1, "./resources/guy2/moai_frame1.png") \
    x(guy_Asset_MoaiFrame2, "./resources/guy2/moai_frame2.png") \
    x(guy_Asset_MoaiFrame3, "./resources/guy2/moai_frame3.png") \
    x(guy_Asset_MoaiEyes1, "./resources/guy2/moai_eyes1.png") \
    x(guy_Asset_MoaiEyes2, "./resources/guy2/moai_eyes2.png") \
    x(guy_Asset_MoaiEyes3, "./resources/guy2/moai_eyes3.png") \
    x(guy_Asset_MoaiMouth1, "./resources/guy2/moai_mouth1.png") \
    x(guy_Asset_MoaiMouth2, "./resources/guy2/moai_mouth2.png") \
    x(guy_Asset_MoaiMouth3, "./resources/guy2/moai_mouth3.png") \
    x(guy_Asset_MoaiNose1, "./resources/guy2/moai_nose1.png") \
    x(guy_Asset_MoaiNose2, "./resources/guy2/moai_nose2.png") \
    x(guy_Asset_MoaiNose3, "./resources/guy2/moai_nose3.png") \
    x(guy_Asset_BunnyFrame1, "./resources/guy2/bunny_frame1.png") \
    x(guy_Asset_BunnyFrame2, "./resources/guy2/bunny_frame2.png") \
    x(guy_Asset_BunnyFrame3, "./resources/guy2/bunny_frame3.png") \
    x(guy_Asset_BunnyFrame4, "./resources/guy2/bunny_frame4.png") \
    x(guy_Asset_BunnyFrame5, "./resources/guy2/bunny_frame5.png") \
    x(guy_Asset_BunnyEyes1, "./resources/guy2/bunny_eyes1.png") \
    x(guy_Asset_BunnyEyes2, "./resources/guy2/bunny_eyes2.png") \
    x(guy_Asset_BunnyMouth1, "./resources/guy2/bunny_mouth1.png") \
    x(guy_Asset_HumanEyes1,      "./resources/guy2/human_eyes1.png") \
    x(guy_Asset_HumanEyes2,      "./resources/guy2/human_eyes2.png") \
    x(guy_Asset_HumanEyes3,      "./resources/guy2/human_eyes3.png") \
    x(guy_Asset_HumanEyes4,      "./resources/guy2/human_eyes4.png") \
    x(guy_Asset_HumanFrame1,     "./resources/guy2/human_frame1.png") \
    x(guy_Asset_HumanFrame2,     "./resources/guy2/human_frame2.png") \
    x(guy_Asset_HumanHair1Front, "./resources/guy2/human_hair1_front.png") \
    x(guy_Asset_HumanHair2Front, "./resources/guy2/human_hair2_front.png") \
    x(guy_Asset_HumanHair3Front, "./resources/guy2/human_hair3_front.png") \
    x(guy_Asset_HumanHair4Front, "./resources/guy2/human_hair4_front.png") \
    x(guy_Asset_HumanHair4Back,  "./resources/guy2/human_hair4_back.png") \
    x(guy_Asset_HumanMouth1,    "./resources/guy2/human_mouth1.png") \
    x(guy_Asset_HumanMouth2,    "./resources/guy2/human_mouth2.png") \
    x(guy_Asset_HumanMouth3,    "./resources/guy2/human_mouth3.png") \
    x(guy_Asset_HumanMouth4,    "./resources/guy2/human_mouth4.png") \
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
    bool crowned;
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
