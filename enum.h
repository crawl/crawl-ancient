enum ABILITIES
{
        ABIL_SPIT_POISON               = 1,   /* 1 */
        ABIL_MAPPING,                     /* 2 */
        ABIL_TELEPORTATION,               /* 3 */
        ABIL_BREATHE_FIRE,                /* 4 */
        ABIL_BLINK,                       /* 5 */
        ABIL_BREATHE_FROST,               /* 6 */
        ABIL_BREATHE_POISON,              /* 7 */
        ABIL_BREATHE_LIGHTNING,           /* 8 */
        ABIL_SPIT_ACID,                   /* 9 */
        ABIL_BREATHE_POWER,               /* 10 */
        ABIL_GO_BERSERK_I,                /* 11 */
        ABIL_BREATHE_STICKY_FLAME,        /* 12 */
        ABIL_BREATHE_STEAM,               /* 13 */
        ABIL_FLY,                         /* 14 */
        ABIL_SUMMON_MINOR_DEMON,          /* 15 */
        ABIL_SUMMON_DEMON,                /* 16 */
        ABIL_HELLFIRE,                    /* 17 */
        ABIL_TORMENT,                     /* 18 */
        ABIL_RAISE_DEAD,                  /* 19 */
        ABIL_CONTROL_DEMON,               /* 20 */
        ABIL_TO_PANDEMONIUM,              /* 21 */
        ABIL_CHANNELING,                  /* 22 */
        ABIL_THROW_FLAME,                 /* 23 */
        ABIL_THROW_FROST,                 /* 24 */
        ABIL_BOLT_OF_DRAINING,            /* 25 */
        ABIL_TURN_INVISIBLE            = 51,   /* 51 */
        ABIL_TURN_VISIBLE,                /* 52 */
        ABIL_LEVITATE,                    /* 53 */
        ABIL_STOP_LEVITATING,             /* 54 */
        ABIL_END_TRANSFORMATION,          /* 55 */
        ABIL_REPEL_UNDEAD_I            = 110,   /* 110 */
        ABIL_MINOR_HEALING,               /* 111 */
        ABIL_PESTILENCE,                  /* 112 */
        ABIL_HOLY_WORD,                   /* 113 */
        ABIL_SUMMON_GUARDIAN,             /* 114 */
        ABIL_REPEL_UNDEAD_II           = 120,   /* 120 */
        ABIL_SMITING,                     /* 121 */
        ABIL_ANNIHILATE_UNDEAD,           /* 122 */
        ABIL_THUNDERBOLT,                 /* 123 */
        ABIL_SUMMON_DAEVA,                /* 124 */
        ABIL_RECALL_UNDEAD_SLAVES      = 130,   /* 130 */
        ABIL_ENSLAVE_UNDEAD            = 132,   /* 132 */
        ABIL_INVOKE_DEATH,                /* 133 */
        ABIL_ANIMATE_CORPSE            = 140,   /* 140 */
        ABIL_RECALL_UNDEAD,               /* 141 */
        ABIL_ANIMATE_DEAD,                /* 142 */
        ABIL_DRAIN_LIFE,                  /* 143 */
        ABIL_CONTROL_UNDEAD,              /* 144 */
        ABIL_CHANNEL_ENERGY            = 160,   /* 160 */
        ABIL_MIGHT_I                   = 170,   /* 170 */
        ABIL_HEALING_I,                   /* 171 */
        ABIL_HASTE,                       /* 172 */
        ABIL_MINOR_DESTRUCTION         = 180,   /* 180 */
        ABIL_LESSER_SERVANT_OF_MAKHLEB,   /* 181 */
        ABIL_MAJOR_DESTRUCTION,           /* 182 */
        ABIL_GREATER_SERVANT_OF_MAKHLEB,   /* 183 */
        ABIL_FORGET_SPELL              = 190,   /* 190 */
        ABIL_GO_BERSERK_II             = 200,   /* 200 */
        ABIL_MIGHT_II,                    /* 201 */
        ABIL_HASTE_SELF,                  /* 202 */
        ABIL_LESSER_HEALING            = 220,   /* 220 */
        ABIL_PURIFICATION,                /* 221 */
        ABIL_HEALING_II,                  /* 222 */
        ABIL_RESTORATION,                 /* 223 */
        ABIL_GREATER_HEALING,             /* 224 */
        ABIL_RENOUNCE_RELIGION         = 250   /* 250 */
};

enum AMMUNITION_DESCRIPTIONS
{
        DAMMO_ORCISH                   = 3,   /* 3 */
        DAMMO_ELVEN,                      /* 4 */
        DAMMO_DWARVEN                    /* 5 */
};

enum ARMOUR
{
        ARM_ROBE,                         /* 0 */
        ARM_LEATHER_ARMOUR,               /* 1 */
        ARM_RING_MAIL,                    /* 2 */
        ARM_SCALE_MAIL,                   /* 3 */
        ARM_CHAIN_MAIL,                   /* 4 */
        ARM_SPLINT_MAIL,                  /* 5 */
        ARM_BANDED_MAIL,                  /* 6 */
        ARM_PLATE_MAIL,                   /* 7 */
        ARM_SHIELD,                       /* 8 */
        ARM_CLOAK,                        /* 9 */
        ARM_HELMET,                       /* 10 */
        ARM_GLOVES,                       /* 11 */
        ARM_BOOTS,                        /* 12 */
        ARM_BUCKLER,                      /* 13 */
        ARM_LARGE_SHIELD,                 /* 14 */
        ARM_DRAGON_HIDE,                  /* 15 */
        ARM_TROLL_HIDE,                   /* 16 */
        ARM_CRYSTAL_PLATE_MAIL,           /* 17 */
        ARM_DRAGON_ARMOUR,                /* 18 */
        ARM_TROLL_LEATHER_ARMOUR,         /* 19 */
        ARM_ICE_DRAGON_HIDE,              /* 20 */
        ARM_ICE_DRAGON_ARMOUR,            /* 21 */
        ARM_STEAM_DRAGON_HIDE,            /* 22 */
        ARM_STEAM_DRAGON_ARMOUR,          /* 23 */
        ARM_MOTTLED_DRAGON_HIDE,          /* 24 */
        ARM_MOTTLED_DRAGON_ARMOUR,        /* 25 */
        ARM_STORM_DRAGON_HIDE,            /* 26 */
        ARM_STORM_DRAGON_ARMOUR,          /* 27 */
        ARM_GOLD_DRAGON_HIDE,             /* 28 */
        ARM_GOLD_DRAGON_ARMOUR,           /* 29 */
        ARM_ANIMAL_SKIN,                  /* 30 */
        ARM_SWAMP_DRAGON_HIDE,            /* 31 */
        ARM_SWAMP_DRAGON_ARMOUR          /* 32 */
};

enum ARMOUR_DESCRIPTIONS
{
        DARM_EMBROIDERED_SHINY         = 1,   /* 1 */
        DARM_RUNED,                       /* 2 */
        DARM_GLOWING,                     /* 3 */
        DARM_ELVEN,                       /* 4 */
        DARM_DWARVEN,                     /* 5 */
        DARM_ORCISH                      /* 6 */
};

enum ARMOUR_PROPERTIES
{
        PARM_AC,                          /* 0 */
        PARM_EVASION                     /* 1 */
};

enum ATTRIBUTES
{
        ATTR_RESIST_LIGHTNING,            /* 0 */
        ATTR_SPEC_AIR,                    /* 1 */
        ATTR_SPEC_EARTH,                  /* 2 */
        ATTR_CONTROL_TELEPORT,            /* 3 */
        ATTR_WALK_SLOWLY,                 /* 4 */
        ATTR_TRANSFORMATION,              /* 5 */
        ATTR_CARD_COUNTDOWN,              /* 6 */
        ATTR_CARD_TABLE,                  /* 7 */
        ATTR_NUM_DEMONIC_POWERS          /* 8 */
};

enum BOOKS
{
        BOOK_MINOR_MAGIC_I,               /* 0 */
        BOOK_MINOR_MAGIC_II,              /* 1 */
        BOOK_MINOR_MAGIC_III,             /* 2 */
        BOOK_CONJURATIONS_I,              /* 3 */
        BOOK_CONJURATIONS_II,             /* 4 */
        BOOK_FLAMES,                      /* 5 */
        BOOK_FROST,                       /* 6 */
        BOOK_INVOCATIONS,                 /* 7 */
        BOOK_FIRE,                        /* 8 */
        BOOK_ICE,                         /* 9 */
        BOOK_SURVEYANCES,                 /* 10 */
        BOOK_SPATIAL_TRANSLOCATIONS,      /* 11 */
        BOOK_ENCHANTMENTS,                /* 12 */
        BOOK_POISONINGS,                  /* 13 */
        BOOK_STORMS_AND_FIRE,             /* 14 */
        BOOK_DEATH,                       /* 15 */
        BOOK_HINDERANCE,                  /* 16 */
        BOOK_CHANGES,                     /* 17 */
        BOOK_TRANSFIGURATIONS,            /* 18 */
        BOOK_USEFUL_MAGIC,                /* 19 */
        BOOK_WAR_CHANTS,                  /* 20 */
        BOOK_CLOUDS,                      /* 21 */
        BOOK_HEALING,                     /* 22 */
        BOOK_NECROMANCY,                  /* 23 */
        BOOK_NECRONOMICON,                /* 24 */
        BOOK_SUMMONINGS,                  /* 25 */
        BOOK_CHARMS,                      /* 26 */
        BOOK_DEMONOLOGY,                  /* 27 */
        BOOK_AIR,                         /* 28 */
        BOOK_SKY,                         /* 29 */
        BOOK_DIVINATIONS,                 /* 30 */
        BOOK_WARP,                        /* 31 */
        BOOK_ENVENOMATIONS,               /* 32 */
        BOOK_ANNIHILATIONS,               /* 33 */
        BOOK_UNLIFE,                      /* 34 */
        BOOK_DESTRUCTION,                 /* 35 */
        BOOK_CONTROL,                     /* 36 */
        BOOK_MUTATIONS,                   /* 37 */
        BOOK_TUKIMA,                      /* 38 */
        BOOK_GEOMANCY,                    /* 39 */
        BOOK_EARTH,                       /* 40 */
        BOOK_MANUAL,                      /* 41 */
        BOOK_WIZARDRY,                    /* 42 */
        BOOK_POWER                       /* 43 */
};

enum DEMON_BEAMS
{
        DMNBM_HELLFIRE,                   /* 0 */
        DMNBM_SMITING,                    /* 1 */
        DMNBM_BRAIN_FEED,                 /* 2 */
        DMNBM_MUTATION                   /* 3 */
};

enum DUNGEON_FEATURES
{
        DNGN_ROCK_WALL                 = 1,   /* 1 */
        DNGN_STONE_WALL,                  /* 2 */
        DNGN_CLOSED_DOOR,                 /* 3 */
        DNGN_METAL_WALL,                  /* 4 */
        DNGN_SECRET_DOOR,                 /* 5 */
        DNGN_GREEN_CRYSTAL_WALL,          /* 6 */
        DNGN_ORCISH_IDOL,                 /* 7 */
        DNGN_WAX_WALL,                    /* 8 */
        DNGN_SILVER_STATUE             = 21,   /* 21 */
        DNGN_GRANITE_STATUE,              /* 22 */
        DNGN_ORANGE_CRYSTAL_STATUE,       /* 23 */
        DNGN_LAVA                      = 61,   /* 61 */
        DNGN_DEEP_WATER,                  /* 62 */
        DNGN_SHALLOW_WATER             = 65,   /* 65 */
        DNGN_FLOOR                     = 67,   /* 67 */
        DNGN_ENTER_HELL                = 69,   /* 69 */
        DNGN_OPEN_DOOR,                   /* 70 */
        DNGN_BRANCH_STAIRS,               /* 71 */
        DNGN_TRAP_I                    = 75,   /* 75 */
        DNGN_TRAP_II,                     /* 76 */
        DNGN_TRAP_III,                    /* 77 */
        DNGN_UNDISCOVERED_TRAP,           /* 78 */
        DNGN_ENTER_SHOP                = 80,   /* 80 */
        DNGN_ENTER_LABYRINTH,             /* 81 */
        DNGN_STONE_STAIRS_DOWN_I,         /* 82 */
        DNGN_STONE_STAIRS_DOWN_II,        /* 83 */
        DNGN_STONE_STAIRS_DOWN_III,       /* 84 */
        DNGN_ROCK_STAIRS_DOWN,            /* 85 */
        DNGN_STONE_STAIRS_UP_I,           /* 86 */
        DNGN_STONE_STAIRS_UP_II,          /* 87 */
        DNGN_STONE_STAIRS_UP_III,         /* 88 */
        DNGN_ROCK_STAIRS_UP,              /* 89 */
        DNGN_ENTER_DIS                 = 92,   /* 92 */
        DNGN_ENTER_GEHENNA,               /* 93 */
        DNGN_ENTER_COCYTUS,               /* 94 */
        DNGN_ENTER_TARTARUS,              /* 95 */
        DNGN_ENTER_ABYSS,                 /* 96 */
        DNGN_EXIT_ABYSS,                  /* 97 */
        DNGN_STONE_ARCH,                  /* 98 */
        DNGN_ENTER_PANDEMONIUM,           /* 99 */
        DNGN_EXIT_PANDEMONIUM,            /* 100 */
        DNGN_TRANSIT_PANDEMONIUM,         /* 101 */
        DNGN_ENTER_ORCISH_MINES        = 110,   /* 110 */
        DNGN_ENTER_HIVE,                  /* 111 */
        DNGN_ENTER_LAIR_I,                /* 112 */
        DNGN_ENTER_SLIME_PITS,            /* 113 */
        DNGN_ENTER_VAULTS,                /* 114 */
        DNGN_ENTER_CRYPT_I,               /* 115 */
        DNGN_ENTER_HALL_OF_BLADES,        /* 116 */
        DNGN_ENTER_ZOT,                   /* 117 */
        DNGN_ENTER_TEMPLE,                /* 118 */
        DNGN_ENTER_SNAKE_PIT,             /* 119 */
        DNGN_ENTER_ELVEN_HALLS,           /* 120 */
        DNGN_ENTER_TOMB,                  /* 121 */
        DNGN_ENTER_SWAMP,                 /* 122 */
        DNGN_RETURN_DUNGEON_I          = 130,   /* 130 */
        DNGN_RETURN_DUNGEON_II,           /* 131 */
        DNGN_RETURN_DUNGEON_III,          /* 132 */
        DNGN_RETURN_LAIR_II,              /* 133 */
        DNGN_RETURN_DUNGEON_IV,           /* 134 */
        DNGN_RETURN_VAULTS,               /* 135 */
        DNGN_RETURN_CRYPT_II,             /* 136 */
        DNGN_EXIT_ZOT,                    /* 137 */
        DNGN_RETURN_DUNGEON_V,            /* 138 */
        DNGN_RETURN_LAIR_III,             /* 139 */
        DNGN_RETURN_MINES,                /* 140 */
        DNGN_RETURN_CRYPT_III,            /* 141 */
        DNGN_RETURN_LAIR_IV,              /* 142 */
        DNGN_ALTAR_ZIN                 = 180,   /* 180 */
        DNGN_ALTAR_SHINING_ONE,           /* 181 */
        DNGN_ALTAR_KIKUBAAQUDGHA,         /* 182 */
        DNGN_ALTAR_YREDELEMNUL,           /* 183 */
        DNGN_ALTAR_XOM,                   /* 184 */
        DNGN_ALTAR_VEHUMET,               /* 185 */
        DNGN_ALTAR_OKAWARU,               /* 186 */
        DNGN_ALTAR_MAKHLEB,               /* 187 */
        DNGN_ALTAR_SIF_MUNA,              /* 188 */
        DNGN_ALTAR_TROG,                  /* 189 */
        DNGN_ALTAR_NEMELEX_XOBEH,         /* 190 */
        DNGN_ALTAR_ELYVILON,              /* 191 */
        DNGN_BLUE_FOUNTAIN             = 200,   /* 200 */
        DNGN_DRY_FOUNTAIN_I,              /* 201 */
        DNGN_SPARKLING_FOUNTAIN,          /* 202 */
        DNGN_DRY_FOUNTAIN_II,             /* 203 */
        DNGN_DRY_FOUNTAIN_III,            /* 204 */
        DNGN_DRY_FOUNTAIN_IV,             /* 205 */
        DNGN_DRY_FOUNTAIN_V,              /* 206 */
        DNGN_DRY_FOUNTAIN_VI             /* 207 */
};

enum DURATIONS
{
        DUR_LIQUID_FLAMES,                /* 0 */
        DUR_ICY_ARMOUR,                   /* 1 */
        DUR_REPEL_MISSILES,               /* 2 */
        DUR_PRAYER,                       /* 3 */
        DUR_REGENERATION,                 /* 4 */
        DUR_VORPAL_BLADE,                 /* 5 */
        DUR_FIRE_BRAND,                   /* 6 */
        DUR_ICE_BRAND,                    /* 7 */
        DUR_LETHAL_INFUSION,              /* 8 */
        DUR_SWIFTNESS,                    /* 9 */
        DUR_INSULATION,                   /* 10 */
        DUR_STONEMAIL,                    /* 11 */
        DUR_CONTROLLED_FLIGHT,            /* 12 */
        DUR_TELEPORT,                     /* 13 */
        DUR_CONTROL_TELEPORT,             /* 14 */
        DUR_POISON_WEAPON,                /* 15 */
        DUR_RESIST_POISON,                /* 16 */
        DUR_BREATH_WEAPON,                /* 17 */
        DUR_TRANSFORMATION,               /* 18 */
        DUR_DEATH_CHANNEL,                /* 19 */
        DUR_DEFLECT_MISSILES             /* 20 */
};

enum EQUIPMENT
{
        EQ_WEAPON,                        /* 0 */
        EQ_CLOAK,                         /* 1 */
        EQ_HELMET,                        /* 2 */
        EQ_GLOVES,                        /* 3 */
        EQ_BOOTS,                         /* 4 */
        EQ_SHIELD,                        /* 5 */
        EQ_BODY_ARMOUR,                   /* 6 */
        EQ_LEFT_RING,                     /* 7 */
        EQ_RIGHT_RING,                    /* 8 */
        EQ_AMULET                        /* 9 */
};

enum FOODS
{
        FOOD_MEAT_RATION,                 /* 0 */
        FOOD_BREAD_RATION,                /* 1 */
        FOOD_PEAR,                        /* 2 */
        FOOD_APPLE,                       /* 3 */
        FOOD_CHOKO,                       /* 4 */
        FOOD_HONEYCOMB,                   /* 5 */
        FOOD_ROYAL_JELLY,                 /* 6 */
        FOOD_SNOZZCUMBER,                 /* 7 */
        FOOD_PIZZA,                       /* 8 */
        FOOD_APRICOT,                     /* 9 */
        FOOD_ORANGE,                      /* 10 */
        FOOD_BANANA,                      /* 11 */
        FOOD_STRAWBERRY,                  /* 12 */
        FOOD_RAMBUTAN,                    /* 13 */
        FOOD_LEMON,                       /* 14 */
        FOOD_GRAPE,                       /* 15 */
        FOOD_SULTANA,                     /* 16 */
        FOOD_LYCHEE,                      /* 17 */
        FOOD_BEEF_JERKY,                  /* 18 */
        FOOD_CHEESE,                      /* 19 */
        FOOD_SAUSAGE,                     /* 20 */
        FOOD_CHUNK                       /* 21 */
};

enum GODS
{
        GOD_NO_GOD,                       /* 0 */
        GOD_ZIN,                          /* 1 */
        GOD_SHINING_ONE,                  /* 2 */
        GOD_KIKUBAAQUDGHA,                /* 3 */
        GOD_YREDELEMNUL,                  /* 4 */
        GOD_XOM,                          /* 5 */
        GOD_VEHUMET,                      /* 6 */
        GOD_OKAWARU,                      /* 7 */
        GOD_MAKHLEB,                      /* 8 */
        GOD_SIF_MUNA,                     /* 9 */
        GOD_TROG,                         /* 10 */
        GOD_NEMELEX_XOBEH,                /* 11 */
        GOD_ELYVILON                     /* 12 */
};

enum HELMET_DESCRIPTIONS
{
        DHELM_WINGED                   = 1,   /* 1 */
        DHELM_HORNED,                     /* 2 */
        DHELM_CRESTED,                    /* 3 */
        DHELM_PLUMED,                     /* 4 */
        DHELM_SPIKED,                     /* 5 */
        DHELM_VISORED,                    /* 6 */
        DHELM_JEWELLED                   /* 7 */
};

enum JEWELLERY
{
        RING_REGENERATION,                /* 0 */
        RING_PROTECTION,                  /* 1 */
        RING_PROTECTION_FROM_FIRE,        /* 2 */
        RING_POISON_RESISTANCE,           /* 3 */
        RING_PROTECTION_FROM_COLD,        /* 4 */
        RING_STRENGTH,                    /* 5 */
        RING_SLAYING,                     /* 6 */
        RING_SEE_INVISIBLE,               /* 7 */
        RING_INVISIBILITY,                /* 8 */
        RING_HUNGER,                      /* 9 */
        RING_TELEPORTATION,               /* 10 */
        RING_EVASION,                     /* 11 */
        RING_SUSTAIN_ABILITIES,           /* 12 */
        RING_SUSTENANCE,                  /* 13 */
        RING_DEXTERITY,                   /* 14 */
        RING_INTELLIGENCE,                /* 15 */
        RING_WIZARDRY,                    /* 16 */
        RING_MAGICAL_POWER,               /* 17 */
        RING_LEVITATION,                  /* 18 */
        RING_LIFE_PROTECTION,             /* 19 */
        RING_PROTECTION_FROM_MAGIC,       /* 20 */
        RING_FIRE,                        /* 21 */
        RING_ICE,                         /* 22 */
        RING_TELEPORT_CONTROL,            /* 23 */
        AMU_RAGE                       = 35,   /* 35 */
        AMU_MAINTAIN_SPEED,               /* 36 */
        AMU_CLARITY,                      /* 37 */
        AMU_WARDING,                      /* 38 */
        AMU_RESIST_CORROSION,             /* 39 */
        AMU_THE_GOURMAND,                 /* 40 */
        AMU_CONSERVATION,                 /* 41 */
        AMU_CONTROLLED_FLIGHT,            /* 42 */
        AMU_INACCURACY,                   /* 43 */
        AMU_RESIST_MUTATION              /* 44 */
};

enum JOB
{
        JOB_FIGHTER,                      /* 0 */
        JOB_WIZARD,                       /* 1 */
        JOB_PRIEST,                       /* 2 */
        JOB_THIEF,                        /* 3 */
        JOB_GLADIATOR,                    /* 4 */
        JOB_NECROMANCER,                  /* 5 */
        JOB_PALADIN,                      /* 6 */
        JOB_ASSASSIN,                     /* 7 */
        JOB_BERSERKER,                    /* 8 */
        JOB_RANGER,                       /* 9 */
        JOB_CONJURER,                     /* 10 */
        JOB_ENCHANTER,                    /* 11 */
        JOB_FIRE_ELEMENTALIST,            /* 12 */
        JOB_ICE_ELEMENTALIST,             /* 13 */
        JOB_SUMMONER,                     /* 14 */
        JOB_AIR_ELEMENTALIST,             /* 15 */
        JOB_EARTH_ELEMENTALIST,           /* 16 */
        JOB_CRUSADER,                     /* 17 */
        JOB_DEATH_KNIGHT,                 /* 18 */
        JOB_VENOM_MAGE,                   /* 19 */
        JOB_CHAOS_KNIGHT,                 /* 20 */
        JOB_TRANSMUTER,                   /* 21 */
        JOB_HEALER,                       /* 22 */
        JOB_REAVER                     = 24,   /* 24 */
        JOB_STALKER,                      /* 25 */
        JOB_MONK,                         /* 26 */
        JOB_WARPER                       /* 27 */
};

enum MISCELLANY
{
        MISC_BOTTLED_EFREET,              /* 0 */
        MISC_CRYSTAL_BALL_OF_SEEING,      /* 1 */
        MISC_AIR_ELEMENTAL_FAN,           /* 2 */
        MISC_LAMP_OF_FIRE,                /* 3 */
        MISC_STONE_OF_EARTH_ELEMENTALS,   /* 4 */
        MISC_LANTERN_OF_SHADOWS,          /* 5 */
        MISC_HORN_OF_GERYON,              /* 6 */
        MISC_BOX_OF_BEASTS,               /* 7 */
        MISC_DECK_OF_WONDERS,             /* 8 */
        MISC_DECK_OF_SUMMONINGS,          /* 9 */
        MISC_CRYSTAL_BALL_OF_ENERGY,      /* 10 */
        MISC_EMPTY_EBONY_CASKET,          /* 11 */
        MISC_CRYSTAL_BALL_OF_FIXATION,    /* 12 */
        MISC_DISC_OF_STORMS,              /* 13 */
        MISC_RUNE_OF_ZOT,                 /* 14 */
        MISC_DECK_OF_TRICKS,              /* 15 */
        MISC_DECK_OF_POWER,               /* 16 */
        MISC_PORTABLE_ALTAR_OF_NEMELEX   /* 17 */
};

enum MISSILES
{
        MI_STONE,                         /* 0 */
        MI_ARROW,                         /* 1 */
        MI_BOLT,                          /* 2 */
        MI_DART,                          /* 3 */
        MI_EGGPLANT,                      /* 4 */
        MI_LARGE_ROCK                    /* 5 */
};

enum MONSTERS
{
        MONS_GIANT_ANT,                   /* 0 */
        MONS_GIANT_BAT,                   /* 1 */
        MONS_CENTAUR,                     /* 2 */
        MONS_RED_DEVIL,                   /* 3 */
        MONS_ETTIN,                       /* 4 */
        MONS_FUNGUS,                      /* 5 */
        MONS_GOBLIN,                      /* 6 */
        MONS_HOUND,                       /* 7 */
        MONS_IMP,                         /* 8 */
        MONS_JACKAL,                      /* 9 */
        MONS_KILLER_BEE,                  /* 10 */
        MONS_KILLER_BEE_LARVA,            /* 11 */
        MONS_MANTICORE,                   /* 12 */
        MONS_NECROPHAGE,                  /* 13 */
        MONS_ORC,                         /* 14 */
        MONS_PHANTOM,                     /* 15 */
        MONS_QUASIT,                      /* 16 */
        MONS_RAT,                         /* 17 */
        MONS_SCORPION,                    /* 18 */
        MONS_TUNNELING_WORM,              /* 19 */
        MONS_UGLY_THING,                  /* 20 */
        MONS_FIRE_VORTEX,                 /* 21 */
        MONS_WORM,                        /* 22 */
        MONS_LARGE_ABOMINATION,           /* 23 */
        MONS_YELLOW_WASP,                 /* 24 */
        MONS_SMALL_ZOMBIE,                /* 25 */
        MONS_ANGEL,                       /* 26 */
        MONS_GIANT_BEETLE,                /* 27 */
        MONS_CYCLOPS,                     /* 28 */
        MONS_DRAGON,                      /* 29 */
        MONS_TWO_HEADED_OGRE,             /* 30 */
        MONS_FIEND,                       /* 31 */
        MONS_GIANT_SPORE,                 /* 32 */
        MONS_HOBGOBLIN,                   /* 33 */
        MONS_ICE_BEAST,                   /* 34 */
        MONS_JELLY,                       /* 35 */
        MONS_KOBOLD,                      /* 36 */
        MONS_LICH,                        /* 37 */
        MONS_MUMMY,                       /* 38 */
        MONS_GUARDIAN_NAGA,               /* 39 */
        MONS_OGRE,                        /* 40 */
        MONS_PLANT,                       /* 41 */
        MONS_QUEEN_BEE,                   /* 42 */
        MONS_RAKSHASA,                    /* 43 */
        MONS_SNAKE,                       /* 44 */
        MONS_TROLL,                       /* 45 */
        MONS_UNSEEN_HORROR,               /* 46 */
        MONS_VAMPIRE,                     /* 47 */
        MONS_WRAITH,                      /* 48 */
        MONS_SMALL_ABOMINATION,           /* 49 */
        MONS_YAK,                         /* 50 */
        MONS_BIG_ZOMBIE,                  /* 51 */
        MONS_ORC_WARRIOR,                 /* 52 */
        MONS_KOBOLD_DEMONOLOGIST,         /* 53 */
        MONS_ORC_WIZARD,                  /* 54 */
        MONS_ORC_KNIGHT,                  /* 55 */
        MONS_WORM_TAIL,                   /* 56 */
        MONS_WYVERN,                      /* 57 */
        MONS_BIG_KOBOLD,                  /* 58 */
        MONS_GIANT_EYEBALL,               /* 59 */
        MONS_WIGHT,                       /* 60 */
        MONS_OKLOB_PLANT,                 /* 61 */
        MONS_WOLF_SPIDER,                 /* 62 */
        MONS_SHADOW,                      /* 63 */
        MONS_HUNGRY_GHOST,                /* 64 */
        MONS_EYE_OF_DRAINING,             /* 65 */
        MONS_BUTTERFLY,                   /* 66 */
        MONS_WANDERING_MUSHROOM,          /* 67 */
        MONS_EFREET,                      /* 68 */
        MONS_BRAIN_WORM,                  /* 69 */
        MONS_GIANT_ORANGE_BRAIN,          /* 70 */
        MONS_BOULDER_BEETLE,              /* 71 */
        MONS_FLYING_SKULL,                /* 72 */
        MONS_HELL_HOUND,                  /* 73 */
        MONS_MINOTAUR,                    /* 74 */
        MONS_ICE_DRAGON,                  /* 75 */
        MONS_SLIME_CREATURE,              /* 76 */
        MONS_FREEZING_WRAITH,             /* 77 */
        MONS_FAKE_RAKSHASA,               /* 78 */
        MONS_GREAT_ORB_OF_EYES,           /* 79 */
        MONS_HELLION,                     /* 80 */
        MONS_ROTTING_DEVIL,               /* 81 */
        MONS_TORMENTOR,                   /* 82 */
        MONS_REAPER,                      /* 83 */
        MONS_SOUL_EATER,                  /* 84 */
        MONS_HAIRY_DEVIL,                 /* 85 */
        MONS_ICE_DEVIL,                   /* 86 */
        MONS_BLUE_DEVIL,                  /* 87 */
        MONS_BEAST,                       /* 88 */
        MONS_IRON_DEVIL,                  /* 89 */
        MONS_GLOWING_SHAPESHIFTER      = 98,   /* 98 */
        MONS_SHAPESHIFTER,                /* 99 */
        MONS_GIANT_MITE,                  /* 100 */
        MONS_STEAM_DRAGON,                /* 101 */
        MONS_VERY_UGLY_THING,             /* 102 */
        MONS_ORC_SORCEROR,                /* 103 */
        MONS_HIPPOGRIFF,                  /* 104 */
        MONS_GRIFFON,                     /* 105 */
        MONS_HYDRA,                       /* 106 */
        MONS_SMALL_SKELETON,              /* 107 */
        MONS_LARGE_SKELETON,              /* 108 */
        MONS_HELL_KNIGHT,                 /* 109 */
        MONS_NECROMANCER,                 /* 110 */
        MONS_WIZARD,                      /* 111 */
        MONS_ORC_PRIEST,                  /* 112 */
        MONS_ORC_HIGH_PRIEST,             /* 113 */
        MONS_HUMAN,                       /* 114 */
        MONS_GNOLL,                       /* 115 */
        MONS_CLAY_GOLEM,                  /* 116 */
        MONS_WOOD_GOLEM,                  /* 117 */
        MONS_STONE_GOLEM,                 /* 118 */
        MONS_IRON_GOLEM,                  /* 119 */
        MONS_CRYSTAL_GOLEM,               /* 120 */
        MONS_TOENAIL_GOLEM,               /* 121 */
        MONS_MOTTLED_DRAGON,              /* 122 */
        MONS_EARTH_ELEMENTAL,             /* 123 */
        MONS_FIRE_ELEMENTAL,              /* 124 */
        MONS_AIR_ELEMENTAL,               /* 125 */
        MONS_ICE_FIEND,                   /* 126 */
        MONS_SHADOW_FIEND,                /* 127 */
        MONS_BROWN_SNAKE,                 /* 128 */
        MONS_GIANT_LIZARD,                /* 129 */
        MONS_SPECTRAL_WARRIOR,            /* 130 */
        MONS_PULSATING_LUMP,              /* 131 */
        MONS_STORM_DRAGON,                /* 132 */
        MONS_YAKTAUR,                     /* 133 */
        MONS_DEATH_YAK,                   /* 134 */
        MONS_ROCK_TROLL,                  /* 135 */
        MONS_STONE_GIANT,                 /* 136 */
        MONS_FLAYED_GHOST,                /* 137 */
        MONS_BUMBLEBEE,                   /* 138 */
        MONS_REDBACK,                     /* 139 */
        MONS_INSUBSTANTIAL_WISP,          /* 140 */
        MONS_VAPOUR,                      /* 141 */
        MONS_OGRE_MAGE,                   /* 142 */
        MONS_SPINY_WORM,                  /* 143 */
        MONS_DANCING_WEAPON,              /* 144 */
        MONS_TITAN,                       /* 145 */
        MONS_GOLDEN_DRAGON,               /* 146 */
        MONS_ELF,                         /* 147 */
        MONS_LINDWORM,                    /* 148 */
        MONS_ELEPHANT_SLUG,               /* 149 */
        MONS_WAR_DOG,                     /* 150 */
        MONS_GREY_RAT,                    /* 151 */
        MONS_GREEN_RAT,                   /* 152 */
        MONS_ORANGE_RAT,                  /* 153 */
        MONS_BLACK_SNAKE,                 /* 154 */
        MONS_SHEEP,                       /* 155 */
        MONS_GHOUL,                       /* 156 */
        MONS_HOG,                         /* 157 */
        MONS_GIANT_MOSQUITO,              /* 158 */
        MONS_GIANT_CENTIPEDE,             /* 159 */
        MONS_IRON_TROLL,                  /* 160 */
        MONS_NAGA,                        /* 161 */
        MONS_FIRE_GIANT,                  /* 162 */
        MONS_FROST_GIANT,                 /* 163 */
        MONS_FIREDRAKE,                   /* 164 */
        MONS_SHADOW_DRAGON,               /* 165 */
        MONS_YELLOW_SNAKE,                /* 166 */
        MONS_GREY_SNAKE,                  /* 167 */
        MONS_DEEP_TROLL,                  /* 168 */
        MONS_GIANT_BLOWFLY,               /* 169 */
        MONS_RED_WASP,                    /* 170 */
        MONS_SWAMP_DRAGON,                /* 171 */
        MONS_SWAMP_DRAKE,                 /* 172 */
        MONS_SOLDIER_ANT,                 /* 173 */
        MONS_HILL_GIANT,                  /* 174 */
        MONS_QUEEN_ANT,                   /* 175 */
        MONS_ANT_LARVA,                   /* 176 */
        MONS_GIANT_FROG,                  /* 177 */
        MONS_GIANT_BROWN_FROG,            /* 178 */
        MONS_SPINY_FROG,                  /* 179 */
        MONS_BLINK_FROG,                  /* 180 */
        MONS_WHITE_IMP                 = 220,   /* 220 */
        MONS_LEMURE,                      /* 221 */
        MONS_UFETUBUS,                    /* 222 */
        MONS_MANES,                       /* 223 */
        MONS_MIDGE,                       /* 224 */
        MONS_NEQOXEC,                     /* 225 */
        MONS_ORANGE_DEMON,                /* 226 */
        MONS_HELLWING,                    /* 227 */
        MONS_SMOKE_DEMON,                 /* 228 */
        MONS_YNOXINUL,                    /* 229 */
        MONS_EXECUTIONER,                 /* 230 */
        MONS_GREEN_DEATH,                 /* 231 */
        MONS_BLUE_DEATH,                  /* 232 */
        MONS_BALRUG,                      /* 233 */
        MONS_CACODEMON,                   /* 234 */
        MONS_DEMONIC_CRAWLER,             /* 235 */
        MONS_SUN_DEMON,                   /* 236 */
        MONS_SHADOW_IMP,                  /* 237 */
        MONS_SHADOW_DEMON,                /* 238 */
        MONS_LOROCYPROCA,                 /* 239 */
        MONS_SHADOW_WRAITH,               /* 240 */
        MONS_GIANT_AMOEBA,                /* 241 */
        MONS_GIANT_SLUG,                  /* 242 */
        MONS_GIANT_SNAIL,                 /* 243 */
        MONS_SPATIAL_VORTEX,              /* 244 */
        MONS_PIT_FIEND,                   /* 245 */
        MONS_BORING_BEETLE,               /* 246 */
        MONS_GARGOYLE,                    /* 247 */
        MONS_METAL_GARGOYLE,              /* 248 */
        MONS_MOLTEN_GARGOYLE,             /* 249 */
        MONS_PROGRAM_BUG,                 /* 250 */
        MONS_MNOLEG,                      /* 251 */
        MONS_LOM_LOBON,                   /* 252 */
        MONS_CEREBOV,                     /* 253 */
        MONS_GLOORX_VLOQ,                 /* 254 */
        MONS_NAGA_MAGE                 = 260,   /* 260 */
        MONS_NAGA_WARRIOR,                /* 261 */
        MONS_ORC_WARLORD,                 /* 262 */
        MONS_DEEP_ELF_SOLDIER,            /* 263 */
        MONS_DEEP_ELF_FIGHTER,            /* 264 */
        MONS_DEEP_ELF_KNIGHT,             /* 265 */
        MONS_DEEP_ELF_MAGE,               /* 266 */
        MONS_DEEP_ELF_SUMMONER,           /* 267 */
        MONS_DEEP_ELF_CONJURER,           /* 268 */
        MONS_DEEP_ELF_PRIEST,             /* 269 */
        MONS_DEEP_ELF_HIGH_PRIEST,        /* 270 */
        MONS_DEEP_ELF_DEMONOLOGIST,       /* 271 */
        MONS_DEEP_ELF_ANNIHILATOR,        /* 272 */
        MONS_DEEP_ELF_SORCEROR,           /* 273 */
        MONS_DEEP_ELF_DEATH_MAGE,         /* 274 */
        MONS_OOZE,                        /* 275 */
        MONS_AZURE_JELLY,                 /* 276 */
        MONS_DEATH_OOZE,                  /* 277 */
        MONS_ACID_BLOB,                   /* 278 */
        MONS_ROYAL_JELLY,                 /* 279 */
        MONS_TERENCE,                     /* 280 */
        MONS_JESSICA,                     /* 281 */
        MONS_IJYB,                        /* 282 */
        MONS_SIGMUND,                     /* 283 */
        MONS_BLORK_THE_ORC,               /* 284 */
        MONS_EDMUND,                      /* 285 */
        MONS_PSYCHE,                      /* 286 */
        MONS_EROLCHA,                     /* 287 */
        MONS_DONALD,                      /* 288 */
        MONS_URUG,                        /* 289 */
        MONS_MICHAEL,                     /* 290 */
        MONS_JOSEPH,                      /* 291 */
        MONS_ANITA,                       /* 292 */
        MONS_ERICA,                       /* 293 */
        MONS_JOSEPHINE,                   /* 294 */
        MONS_HAROLD,                      /* 295 */
        MONS_NORBERT,                     /* 296 */
        MONS_JOZEF,                       /* 297 */
        MONS_AGNES,                       /* 298 */
        MONS_MAUD,                        /* 299 */
        MONS_LOUISE,                      /* 300 */
        MONS_FRANCIS,                     /* 301 */
        MONS_FRANCES,                     /* 302 */
        MONS_RUPERT,                      /* 303 */
        MONS_WAYNE,                       /* 304 */
        MONS_DUANE,                       /* 305 */
        MONS_XTAHUA,                      /* 306 */
        MONS_NORRIS,                      /* 307 */
        MONS_ADOLF,                       /* 308 */
        MONS_MARGERY,                     /* 309 */
        MONS_BORIS,                       /* 310 */
        MONS_GERYON                    = 340,   /* 340 */
        MONS_DISPATER,                    /* 341 */
        MONS_ASMODEUS,                    /* 342 */
        MONS_ANTAEUS,                     /* 343 */
        MONS_ERESHKIGAL,                  /* 344 */
        MONS_ANCIENT_LICH              = 356,   /* 356 */
        MONS_VAULT_GUARD               = 360,   /* 360 */
        MONS_CURSE_SKULL,                 /* 361 */
        MONS_VAMPIRE_KNIGHT,              /* 362 */
        MONS_VAMPIRE_MAGE,                /* 363 */
        MONS_SHINING_EYE,                 /* 364 */
        MONS_ORB_GUARDIAN,                /* 365 */
        MONS_DAEVA,                       /* 366 */
        MONS_SPECTRAL_THING,              /* 367 */
        MONS_GREATER_NAGA,                /* 368 */
        MONS_SKELETAL_DRAGON,             /* 369 */
        MONS_TENTACLED_MONSTROSITY,       /* 370 */
        MONS_SPHINX,                      /* 371 */
        MONS_ROTTING_HULK,                /* 372 */
        MONS_GUARDIAN_MUMMY,              /* 373 */
        MONS_GREATER_MUMMY,               /* 374 */
        MONS_MUMMY_PRIEST,                /* 375 */
        MONS_CENTAUR_WARRIOR,             /* 376 */
        MONS_YAKTAUR_CAPTAIN,             /* 377 */
        MONS_KILLER_KLOWN,                /* 378 */
        MONS_GUARDIAN_ROBOT,              /* 379 */
        MONS_DORGI,                       /* 380 */
        MONS_SWORD,                       /* 381 */
        MONS_QUOKKA,                      /* 382 */
        MONS_EYE_OF_DEVASTATION        = 385,   /* 385 */
        MONS_MOTH_OF_WRATH,               /* 386 */
        MONS_DEATH_COB,                   /* 387 */
        MONS_CURSE_TOE,                   /* 388 */
        MONS_GOLD_MIMIC,                  /* 389 */
        MONS_WEAPON_MIMIC,                /* 390 */
        MONS_ARMOUR_MIMIC,                /* 391 */
        MONS_SCROLL_MIMIC,                /* 392 */
        MONS_POTION_MIMIC,                /* 393 */
        MONS_HELL_HOG,                    /* 394 */
        MONS_SERPENT_OF_HELL,             /* 395 */
        MONS_BOGGART,                     /* 396 */
        MONS_QUICKSILVER_DRAGON,          /* 397 */
        MONS_IRON_DRAGON,                 /* 398 */
        MONS_SKELETAL_WARRIOR,            /* 399 */
        MONS_PLAYER_GHOST,                /* 400 */
        MONS_PANDEMONIUM_DEMON,           /* 401 */
        MONS_LAVA_WORM                 = 420,   /* 420 */
        MONS_LAVA_FISH,                   /* 421 */
        MONS_LAVA_SNAKE,                  /* 422 */
        MONS_ANOTHER_LAVA_THING,          /* 423 */
        MONS_BIG_FISH                  = 430,   /* 430 */
        MONS_GIANT_GOLDFISH,              /* 431 */
        MONS_ELECTRICAL_EEL,              /* 432 */
        MONS_JELLYFISH,                   /* 433 */
        MONS_WATER_ELEMENTAL,             /* 434 */
        MONS_SWAMP_WORM                  /* 435 */
};

enum MONSTER_BEHAVIOR
{
        BEH_SLEEP,                        /* 0 */
        BEH_CHASING_I,                    /* 1 */
        BEH_WANDER,                       /* 2 */
        BEH_FLEE,                         /* 3 */
        BEH_CONFUSED,                     /* 4 */
        BEH_FIGHT                      = 6,   /* 6 */
        BEH_ENSLAVED,                     /* 7 */
        BEH_CHASING_II                 = 100   /* 100 */
};

enum MONSTER_HOLINESS
{
        MH_NORMAL,                        /* 0 */
        MH_UNDEAD,                        /* 1 */
        MH_DEMONIC                       /* 2 */
};

enum MUTATIONS
{
        MUT_TOUGH_SKIN,                   /* 0 */
        MUT_STRONG,                       /* 1 */
        MUT_CLEVER,                       /* 2 */
        MUT_AGILE,                        /* 3 */
        MUT_GREEN_SCALES,                 /* 4 */
        MUT_BLACK_SCALES,                 /* 5 */
        MUT_GREY_SCALES,                  /* 6 */
        MUT_BONEY_PLATES,                 /* 7 */
        MUT_REPULSION_FIELD,              /* 8 */
        MUT_POISON_RESISTANCE,            /* 9 */
        MUT_CARNIVOROUS,                  /* 10 */
        MUT_HERBIVOROUS,                  /* 11 */
        MUT_HEAT_RESISTANCE,              /* 12 */
        MUT_COLD_RESISTANCE,              /* 13 */
        MUT_SHOCK_RESISTANCE,             /* 14 */
        MUT_REGENERATION,                 /* 15 */
        MUT_FAST_METABOLISM,              /* 16 */
        MUT_SLOW_METABOLISM,              /* 17 */
        MUT_WEAK,                         /* 18 */
        MUT_DOPEY,                        /* 19 */
        MUT_CLUMSY,                       /* 20 */
        MUT_TELEPORT_CONTROL,             /* 21 */
        MUT_TELEPORT,                     /* 22 */
        MUT_MAGIC_RESISTANCE,             /* 23 */
        MUT_FAST,                         /* 24 */
        MUT_ACUTE_VISION,                 /* 25 */
        MUT_DEFORMED,                     /* 26 */
        MUT_TELEPORT_AT_WILL,             /* 27 */
        MUT_SPIT_POISON,                  /* 28 */
        MUT_MAPPING,                      /* 29 */
        MUT_BREATHE_FLAMES,               /* 30 */
        MUT_BLINK,                        /* 31 */
        MUT_HORNS,                        /* 32 */
        MUT_STRONG_STIFF,                 /* 33 */
        MUT_FLEXIBLE_WEAK,                /* 34 */
        MUT_LOST,                         /* 35 */
        MUT_CLARITY,                      /* 36 */
        MUT_BERSERK,                      /* 37 */
        MUT_DETERIORATION,                /* 38 */
        MUT_BLURRY_VISION,                /* 39 */
        MUT_MUTATION_RESISTANCE,          /* 40 */
        MUT_FRAIL,                        /* 41 */
        MUT_ROBUST,                       /* 42 */
        MUT_TORMENT_RESISTANCE,           /* 43 */
        MUT_NEGATIVE_ENERGY_RESISTANCE,   /* 44 */
        MUT_SUMMON_MINOR_DEMONS,          /* 45 */
        MUT_SUMMON_DEMONS,                /* 46 */
        MUT_HURL_HELLFIRE,                /* 47 */
        MUT_CALL_TORMENT,                 /* 48 */
        MUT_RAISE_DEAD,                   /* 49 */
        MUT_CONTROL_DEMONS,               /* 50 */
        MUT_PANDEMONIUM,                  /* 51 */
        MUT_DEATH_STRENGTH,               /* 52 */
        MUT_CHANNEL_HELL,                 /* 53 */
        MUT_DRAIN_LIFE,                   /* 54 */
        MUT_THROW_FLAMES,                 /* 55 */
        MUT_THROW_FROST,                  /* 56 */
        MUT_SMITE                        /* 57 */
};

enum NAMED_WEAPONS
{
        NWPN_SINGING_SWORD             = 181,   /* 181 */
        NWPN_WRATH_OF_TROG,               /* 182 */
        NWPN_SCYTHE_OF_CURSES,            /* 183 */
        NWPN_MACE_OF_VARIABILITY,         /* 184 */
        NWPN_GLAIVE_OF_PRUNE,             /* 185 */
        NWPN_SCEPTRE_OF_TORMENT,          /* 186 */
        NWPN_SWORD_OF_ZONGULDROK,         /* 187 */
        NWPN_SWORD_OF_CEREBOV,            /* 188 */
        NWPN_STAFF_OF_DISPATER,           /* 189 */
        NWPN_SCEPTRE_OF_ASMODEUS,         /* 190 */
        NWPN_SWORD_OF_POWER,              /* 191 */
        NWPN_KNIFE_OF_ACCURACY,           /* 192 */
        NWPN_STAFF_OF_OLGREB,             /* 193 */
        NWPN_VAMPIRE_S_TOOTH,             /* 194 */
        NWPN_STAFF_OF_WUCAD_MU           /* 195 */
};

enum NAUGHTY_THINGS
{
        NAUGHTY_NECROMANCY             = 1,   /* 1 */
        NAUGHTY_UNHOLY,                   /* 2 */
        NAUGHTY_KILLING,                  /* 3 */
        NAUGHTY_ATTACK_HOLY,              /* 4 */
        NAUGHTY_ATTACK_FRIEND,            /* 5 */
        NAUGHTY_FRIEND_DIES,              /* 6 */
        NAUGHTY_BUTCHER,                  /* 7 */
        NAUGHTY_STABBING,                 /* 8 */
        NAUGHTY_SPELLCASTING             /* 9 */
};

enum OBJECT_CLASSES
{
        OBJ_WEAPONS,                      /* 0 */
        OBJ_MISSILES,                     /* 1 */
        OBJ_ARMOUR,                       /* 2 */
        OBJ_WANDS,                        /* 3 */
        OBJ_FOOD,                         /* 4 */
        OBJ_SCROLLS                    = 6,   /* 6 */
        OBJ_JEWELLERY,                    /* 7 */
        OBJ_POTIONS,                      /* 8 */
        OBJ_BOOKS                      = 10,   /* 10 */
        OBJ_STAVES,                       /* 11 */
        OBJ_ORBS,                         /* 12 */
        OBJ_MISCELLANY,                   /* 13 */
        OBJ_CORPSES                      /* 14 */
};

enum ORBS
{
        ORB_ZOT                          /* 0 */
};

enum POTIONS
{
        POT_HEALING,                      /* 0 */
        POT_HEAL_WOUNDS,                  /* 1 */
        POT_SPEED,                        /* 2 */
        POT_MIGHT,                        /* 3 */
        POT_GAIN_STRENGTH,                /* 4 */
        POT_GAIN_DEXTERITY,               /* 5 */
        POT_GAIN_INTELLIGENCE,            /* 6 */
        POT_LEVITATION,                   /* 7 */
        POT_POISON,                       /* 8 */
        POT_SLOWING,                      /* 9 */
        POT_PARALYSIS,                    /* 10 */
        POT_CONFUSION,                    /* 11 */
        POT_INVISIBILITY,                 /* 12 */
        POT_PORRIDGE,                     /* 13 */
        POT_DEGENERATION,                 /* 14 */
        POT_DECAY,                        /* 15 */
        POT_WATER,                        /* 16 */
        POT_EXPERIENCE,                   /* 17 */
        POT_MAGIC,                        /* 18 */
        POT_RESTORE_ABILITIES,            /* 19 */
        POT_STRONG_POISON,                /* 20 */
        POT_BERSERK_RAGE,                 /* 21 */
        POT_CURE_MUTATION,                /* 22 */
        POT_MUTATION                     /* 23 */
};

enum RANDART_PROP
{
        RAP_BRAND,                        /* 0 */
        RAP_AC,                           /* 1 */
        RAP_EVASION,                      /* 2 */
        RAP_STRENGTH,                     /* 3 */
        RAP_INTELLIGENCE,                 /* 4 */
        RAP_DEXTERITY,                    /* 5 */
        RAP_FIRE,                         /* 6 */
        RAP_COLD,                         /* 7 */
        RAP_ELECTRICITY,                  /* 8 */
        RAP_POISON,                       /* 9 */
        RAP_NEGATIVE_ENERGY,              /* 10 */
        RAP_MAGIC,                        /* 11 */
        RAP_EYESIGHT,                     /* 12 */
        RAP_INVISIBLE,                    /* 13 */
        RAP_LEVITATE,                     /* 14 */
        RAP_BLINK,                        /* 15 */
        RAP_CAN_TELEPORT,                 /* 16 */
        RAP_BERSERK,                      /* 17 */
        RAP_MAPPING,                      /* 18 */
        RAP_NOISES,                       /* 19 */
        RAP_PREVENT_SPELLCASTING,         /* 20 */
        RAP_CAUSE_TELEPORTATION,          /* 21 */
        RAP_PREVENT_TELEPORTATION,        /* 22 */
        RAP_ANGRY,                        /* 23 */
        RAP_METABOLISM,                   /* 24 */
        RAP_MUTAGENIC,                    /* 25 */
        RAP_ACCURACY,                     /* 26 */
        RAP_DAMAGE                       /* 27 */
};

enum SCROLLS
{
        SCR_IDENTIFY,                     /* 0 */
        SCR_TELEPORTATION,                /* 1 */
        SCR_FEAR,                         /* 2 */
        SCR_NOISE,                        /* 3 */
        SCR_REMOVE_CURSE,                 /* 4 */
        SCR_DETECT_CURSE,                 /* 5 */
        SCR_SUMMONING,                    /* 6 */
        SCR_ENCHANT_WEAPON_I,             /* 7 */
        SCR_ENCHANT_ARMOUR,               /* 8 */
        SCR_TORMENT,                      /* 9 */
        SCR_RANDOM_USELESSNESS,           /* 10 */
        SCR_CURSE_WEAPON,                 /* 11 */
        SCR_CURSE_ARMOUR,                 /* 12 */
        SCR_IMMOLATION,                   /* 13 */
        SCR_BLINKING,                     /* 14 */
        SCR_PAPER,                        /* 15 */
        SCR_MAGIC_MAPPING,                /* 16 */
        SCR_FORGETFULNESS,                /* 17 */
        SCR_ACQUIREMENT,                  /* 18 */
        SCR_ENCHANT_WEAPON_II,            /* 19 */
        SCR_VORPALISE_WEAPON,             /* 20 */
        SCR_RECHARGING,                   /* 21 */
        SCR_ENCHANT_WEAPON_III           /* 22 */
};

enum SKILLS
{
        SK_FIGHTING,                      /* 0 */
        SK_SHORT_BLADES,                  /* 1 */
        SK_LONG_SWORDS,                   /* 2 */
        SK_GREAT_SWORDS,                  /* 3 */
        SK_AXES,                          /* 4 */
        SK_MACES_FLAILS,                  /* 5 */
        SK_POLEARMS,                      /* 6 */
        SK_STAVES,                        /* 7 */
        SK_SLINGS,                        /* 8 */
        SK_BOWS,                          /* 9 */
        SK_CROSSBOWS,                     /* 10 */
        SK_DARTS,                         /* 11 */
        SK_THROWING,                      /* 12 */
        SK_ARMOUR,                        /* 13 */
        SK_DODGING,                       /* 14 */
        SK_STEALTH,                       /* 15 */
        SK_STABBING,                      /* 16 */
        SK_SHIELDS,                       /* 17 */
        SK_TRAPS_DOORS,                   /* 18 */
        SK_UNARMED_COMBAT,                /* 19 */
        SK_SPELLCASTING                = 25,   /* 25 */
        SK_CONJURATIONS,                  /* 26 */
        SK_ENCHANTMENTS,                  /* 27 */
        SK_SUMMONINGS,                    /* 28 */
        SK_NECROMANCY,                    /* 29 */
        SK_TRANSLOCATIONS,                /* 30 */
        SK_TRANSMIGRATION,                /* 31 */
        SK_DIVINATIONS,                   /* 32 */
        SK_FIRE_MAGIC,                    /* 33 */
        SK_ICE_MAGIC,                     /* 34 */
        SK_AIR_MAGIC,                     /* 35 */
        SK_EARTH_MAGIC,                   /* 36 */
        SK_POISON_MAGIC,                  /* 37 */
        SK_INVOCATIONS                   /* 38 */
};

enum SPECIAL_ARMOR
{
        SPARM_NORMAL,                     /* 0 */
        SPARM_RUNNING,                    /* 1 */
        SPARM_FIRE_RESISTANCE,            /* 2 */
        SPARM_COLD_RESISTANCE,            /* 3 */
        SPARM_POISON_RESISTANCE,          /* 4 */
        SPARM_SEE_INVISIBLE,              /* 5 */
        SPARM_DARKNESS,                   /* 6 */
        SPARM_STRENGTH,                   /* 7 */
        SPARM_DEXTERITY,                  /* 8 */
        SPARM_INTELLIGENCE,               /* 9 */
        SPARM_PONDEROUSNESS,              /* 10 */
        SPARM_LEVITATION,                 /* 11 */
        SPARM_MAGIC_RESISTANCE,           /* 12 */
        SPARM_PROTECTION,                 /* 13 */
        SPARM_STEALTH,                    /* 14 */
        SPARM_RESISTANCE,                 /* 15 */
        SPARM_POSITIVE_ENERGY,            /* 16 */
        SPARM_ARCHMAGI,                   /* 17 */
        SPARM_PRESERVATION               /* 18 */
};

enum SPECIAL_WEAPONS
{
        SPWPN_NORMAL,                     /* 0 */
        SPWPN_FLAMING,                    /* 1 */
        SPWPN_FREEZING,                   /* 2 */
        SPWPN_HOLY_WRATH,                 /* 3 */
        SPWPN_ELECTROCUTION,              /* 4 */
        SPWPN_ORC_SLAYING,                /* 5 */
        SPWPN_VENOM,                      /* 6 */
        SPWPN_PROTECTION,                 /* 7 */
        SPWPN_DRAINING,                   /* 8 */
        SPWPN_SPEED,                      /* 9 */
        SPWPN_VORPAL,                     /* 10 */
        SPWPN_FLAME,                      /* 11 */
        SPWPN_FROST,                      /* 12 */
        SPWPN_VAMPIRICISM,                /* 13 */
        SPWPN_DISRUPTION,                 /* 14 */
        SPWPN_PAIN,                       /* 15 */
        SPWPN_DISTORTION,                 /* 16 */
        SPWPN_RANDART_I                = 25,   /* 25 */
        SPWPN_RANDART_II,                 /* 26 */
        SPWPN_RANDART_III,                /* 27 */
        SPWPN_RANDART_IV,                 /* 28 */
        SPWPN_RANDART_V                  /* 29 */
};

enum SPECIAL_WIELD
{
        SPWLD_NONE,                       /* 0 */
        SPWLD_SING,                       /* 1 */
        SPWLD_TROG,                       /* 2 */
        SPWLD_CURSE,                      /* 3 */
        SPWLD_VARIABLE,                   /* 4 */
        SPWLD_TORMENT                  = 6,   /* 6 */
        SPWLD_ZONGULDROK,                 /* 7 */
        SPWLD_POWER,                      /* 8 */
        SPWLD_WUCAD_MU,                   /* 9 */
        SPWLD_SHADOW                   = 50   /* 50 */
};

enum SPECIES
{
        SP_HUMAN                       = 1,   /* 1 */
        SP_ELF,                           /* 2 */
        SP_HIGH_ELF,                      /* 3 */
        SP_GREY_ELF,                      /* 4 */
        SP_DEEP_ELF,                      /* 5 */
        SP_SLUDGE_ELF,                    /* 6 */
        SP_HILL_DWARF,                    /* 7 */
        SP_MOUNTAIN_DWARF,                /* 8 */
        SP_HALFLING,                      /* 9 */
        SP_HILL_ORC,                      /* 10 */
        SP_KOBOLD,                        /* 11 */
        SP_MUMMY,                         /* 12 */
        SP_NAGA,                          /* 13 */
        SP_GNOME,                         /* 14 */
        SP_OGRE,                          /* 15 */
        SP_TROLL,                         /* 16 */
        SP_OGRE_MAGE,                     /* 17 */
        SP_RED_DRACONIAN,                 /* 18 */
        SP_WHITE_DRACONIAN,               /* 19 */
        SP_GREEN_DRACONIAN,               /* 20 */
        SP_GOLDEN_DRACONIAN,              /* 21 */
        SP_GREY_DRACONIAN,                /* 22 */
        SP_BLACK_DRACONIAN,               /* 23 */
        SP_PURPLE_DRACONIAN,              /* 24 */
        SP_MOTTLED_DRACONIAN,             /* 25 */
        SP_PALE_DRACONIAN,                /* 26 */
        SP_UNK0_DRACONIAN,                /* 27 */
        SP_UNK1_DRACONIAN,                /* 28 */
        SP_UNK2_DRACONIAN,                /* 29 */
        SP_CENTAUR,                       /* 30 */
        SP_DEMIGOD,                       /* 31 */
        SP_SPRIGGAN,                      /* 32 */
        SP_MINOTAUR,                      /* 33 */
        SP_DEMONSPAWN,                    /* 34 */
        SP_GHOUL,                         /* 35 */
        SP_KENKU                         /* 36 */
};

enum SPELLS
{
        SPELL_IDENTIFY,                   /* 0 */
        SPELL_TELEPORT_SELF,              /* 1 */
        SPELL_CAUSE_FEAR,                 /* 2 */
        SPELL_CREATE_NOISE,               /* 3 */
        SPELL_REMOVE_CURSE,               /* 4 */
        SPELL_MAGIC_DART,                 /* 5 */
        SPELL_FIREBALL,                   /* 6 */
        SPELL_CONJURE_FLAME            = 13,   /* 13 */
        SPELL_DIG,                        /* 14 */
        SPELL_BOLT_OF_FIRE,               /* 15 */
        SPELL_BOLT_OF_COLD,               /* 16 */
        SPELL_LIGHTNING_BOLT,             /* 17 */
        SPELL_POLYMORPH_OTHER          = 20,   /* 20 */
        SPELL_SLOW,                       /* 21 */
        SPELL_HASTE,                      /* 22 */
        SPELL_PARALYZE,                   /* 23 */
        SPELL_CONFUSE,                    /* 24 */
        SPELL_INVISIBILITY,               /* 25 */
        SPELL_THROW_FLAME,                /* 26 */
        SPELL_THROW_FROST,                /* 27 */
        SPELL_CONTROLLED_BLINK,           /* 28 */
        SPELL_FREEZING_CLOUD,             /* 29 */
        SPELL_MEPHITIC_CLOUD,             /* 30 */
        SPELL_RING_OF_FLAMES,             /* 31 */
        SPELL_RESTORE_STRENGTH,           /* 32 */
        SPELL_RESTORE_INTELLIGENCE,       /* 33 */
        SPELL_RESTORE_DEXTERITY,          /* 34 */
        SPELL_VENOM_BOLT,                 /* 35 */
        SPELL_OLGREBS_TOXIC_RADIANCE,     /* 36 */
        SPELL_TELEPORT_OTHER,             /* 37 */
        SPELL_LESSER_HEALING,             /* 38 */
        SPELL_GREATER_HEALING,            /* 39 */
        SPELL_CURE_POISON_I,              /* 40 */
        SPELL_PURIFICATION,               /* 41 */
        SPELL_DEATHS_DOOR,                /* 42 */
        SPELL_SELECTIVE_AMNESIA,          /* 43 */
        SPELL_MASS_CONFUSION,             /* 44 */
        SPELL_SMITING,                    /* 45 */
        SPELL_REPEL_UNDEAD,               /* 46 */
        SPELL_HOLY_WORD,                  /* 47 */
        SPELL_DETECT_CURSE,               /* 48 */
        SPELL_SUMMON_SMALL_MAMMAL,        /* 49 */
        SPELL_ABJURATION_I,               /* 50 */
        SPELL_SUMMON_SCORPIONS,           /* 51 */
        SPELL_LEVITATION,                 /* 52 */
        SPELL_BOLT_OF_DRAINING,           /* 53 */
        SPELL_LEHUDIBS_CRYSTAL_SPEAR,     /* 54 */
        SPELL_BOLT_OF_INACCURACY,         /* 55 */
        SPELL_POISONOUS_CLOUD,            /* 56 */
        SPELL_FIRE_STORM,                 /* 57 */
        SPELL_DETECT_TRAPS,               /* 58 */
        SPELL_BLINK,                      /* 59 */
        SPELL_ISKENDERUNS_MYSTIC_BLAST,   /* 60 */
        SPELL_SWARM,                      /* 61 */
        SPELL_SUMMON_HORRIBLE_THINGS,     /* 62 */
        SPELL_ENSLAVEMENT,                /* 63 */
        SPELL_MAGIC_MAPPING,              /* 64 */
        SPELL_HEAL_OTHER,                 /* 65 */
        SPELL_ANIMATE_DEAD,               /* 66 */
        SPELL_PAIN,                       /* 67 */
        SPELL_EXTENSION,                  /* 68 */
        SPELL_CONTROL_UNDEAD,             /* 69 */
        SPELL_ANIMATE_SKELETON,           /* 70 */
        SPELL_VAMPIRIC_DRAINING,          /* 71 */
        SPELL_SUMMON_WRAITHS,             /* 72 */
        SPELL_DETECT_ITEMS,               /* 73 */
        SPELL_BORGNJORS_REVIVIFICATION,   /* 74 */
        SPELL_BURN,                       /* 75 */
        SPELL_FREEZE,                     /* 76 */
        SPELL_SUMMON_ELEMENTAL,           /* 77 */
        SPELL_OZOCUBUS_REFRIGERATION,     /* 78 */
        SPELL_STICKY_FLAME,               /* 79 */
        SPELL_SUMMON_ICE_BEAST,           /* 80 */
        SPELL_OZOCUBUS_ARMOUR,            /* 81 */
        SPELL_CALL_IMP,                   /* 82 */
        SPELL_REPEL_MISSILES,             /* 83 */
        SPELL_BERSERKER_RAGE,             /* 84 */
        SPELL_DISPEL_UNDEAD,              /* 85 */
        SPELL_GUARDIAN,                   /* 86 */
        SPELL_PESTILENCE,                 /* 87 */
        SPELL_THUNDERBOLT,                /* 88 */
        SPELL_FLAME_OF_CLEANSING,         /* 89 */
        SPELL_SHINING_LIGHT,              /* 90 */
        SPELL_SUMMON_DAEVA,               /* 91 */
        SPELL_ABJURATION_II,              /* 92 */
        SPELL_TWISTED_RESURRECTION     = 110,   /* 110 */
        SPELL_REGENERATION,               /* 111 */
        SPELL_BONE_SHARDS,                /* 112 */
        SPELL_BANISHMENT,                 /* 113 */
        SPELL_CIGOTUVIS_DEGENERATION,     /* 114 */
        SPELL_STING,                      /* 115 */
        SPELL_SUBLIMATION_OF_BLOOD,       /* 116 */
        SPELL_TUKIMAS_DANCE,              /* 117 */
        SPELL_HELLFIRE,                   /* 118 */
        SPELL_SUMMON_DEMON,               /* 119 */
        SPELL_DEMONIC_HORDE,              /* 120 */
        SPELL_SUMMON_GREATER_DEMON,       /* 121 */
        SPELL_CORPSE_ROT,                 /* 122 */
        SPELL_TUKIMAS_VORPAL_BLADE,       /* 123 */
        SPELL_FIRE_BRAND,                 /* 124 */
        SPELL_FREEZING_AURA,              /* 125 */
        SPELL_LETHAL_INFUSION,            /* 126 */
        SPELL_CRUSH,                      /* 127 */
        SPELL_BOLT_OF_IRON,               /* 128 */
        SPELL_STONE_ARROW,                /* 129 */
        SPELL_TOMB_OF_DOROKLOHE,          /* 130 */
        SPELL_STONEMAIL,                  /* 131 */
        SPELL_SHOCK,                      /* 132 */
        SPELL_SWIFTNESS,                  /* 133 */
        SPELL_FLY,                        /* 134 */
        SPELL_INSULATION,                 /* 135 */
        SPELL_ORB_OF_ELECTROCUTION,       /* 136 */
        SPELL_DETECT_CREATURES,           /* 137 */
        SPELL_CURE_POISON_II,             /* 138 */
        SPELL_CONTROL_TELEPORT,           /* 139 */
        SPELL_POISON_AMMUNITION,          /* 140 */
        SPELL_POISON_WEAPON,              /* 141 */
        SPELL_RESIST_POISON,              /* 142 */
        SPELL_PROJECTED_NOISE,            /* 143 */
        SPELL_ALTER_SELF,                 /* 144 */
        SPELL_DEBUGGING_RAY,              /* 145 */
        SPELL_RECALL,                     /* 146 */
        SPELL_PORTAL,                     /* 147 */
        SPELL_AGONY,                      /* 148 */
        SPELL_SPIDER_FORM,                /* 149 */
        SPELL_DISRUPT,                    /* 150 */
        SPELL_DISINTEGRATE,               /* 151 */
        SPELL_BLADE_HANDS,                /* 152 */
        SPELL_STATUE_FORM,                /* 153 */
        SPELL_ICE_FORM,                   /* 154 */
        SPELL_DRAGON_FORM,                /* 155 */
        SPELL_NECROMUTATION,              /* 156 */
        SPELL_DEATH_CHANNEL,              /* 157 */
        SPELL_SYMBOL_OF_TORMENT,          /* 158 */
        SPELL_DEFLECT_MISSILES,           /* 159 */
        SPELL_ORB_OF_FRAGMENTATION,       /* 160 */
        SPELL_ICE_BOLT,                   /* 161 */
        SPELL_ICE_STORM,                  /* 162 */
        SPELL_ARC,                        /* 163 */
        SPELL_AIRSTRIKE,                  /* 164 */
        SPELL_SHADOW_CREATURES           /* 165 */
};

enum SPELL_TYPES
{
        SPTYP_CONJURATION              = 11,   /* 11 */
        SPTYP_ENCHANTMENT,                /* 12 */
        SPTYP_FIRE,                       /* 13 */
        SPTYP_ICE,                        /* 14 */
        SPTYP_TRANSMIGRATION,             /* 15 */
        SPTYP_NECROMANCY,                 /* 16 */
        SPTYP_HOLY,                       /* 17 */
        SPTYP_SUMMONING,                  /* 18 */
        SPTYP_DIVINATION,                 /* 19 */
        SPTYP_TRANSLOCATION,              /* 20 */
        SPTYP_POISON,                     /* 21 */
        SPTYP_EARTH,                      /* 22 */
        SPTYP_AIR                        /* 23 */
};

enum STATS
{
        STAT_STRENGTH,                    /* 0 */
        STAT_DEXTERITY,                   /* 1 */
        STAT_INTELLIGENCE                /* 2 */
};

enum STAVES
{
        STAFF_WIZARDRY,                   /* 0 */
        STAFF_POWER,                      /* 1 */
        STAFF_FIRE,                       /* 2 */
        STAFF_COLD,                       /* 3 */
        STAFF_POISON,                     /* 4 */
        STAFF_ENERGY,                     /* 5 */
        STAFF_DEATH,                      /* 6 */
        STAFF_CONJURATION,                /* 7 */
        STAFF_ENCHANTMENT,                /* 8 */
        STAFF_SUMMONING_I,                /* 9 */
        STAFF_SMITING,                    /* 10 */
        STAFF_SUMMONING_II,               /* 11 */
        STAFF_DESTRUCTION_I,              /* 12 */
        STAFF_DESTRUCTION_II,             /* 13 */
        STAFF_DESTRUCTION_III,            /* 14 */
        STAFF_DESTRUCTION_IV,             /* 15 */
        STAFF_WARDING,                    /* 16 */
        STAFF_DISCOVERY,                  /* 17 */
        STAFF_DEMONOLOGY,                 /* 18 */
        STAFF_AIR                      = 25,   /* 25 */
        STAFF_EARTH,                      /* 26 */
        STAFF_CHANNELING                 /* 27 */
};

enum TRANSFORMATIONS
{
        TRAN_NONE,                        /* 0 */
        TRAN_SPIDER,                      /* 1 */
        TRAN_BLADE_HANDS,                 /* 2 */
        TRAN_STATUE,                      /* 3 */
        TRAN_ICE_BEAST,                   /* 4 */
        TRAN_DRAGON,                      /* 5 */
        TRAN_LICH                        /* 6 */
};

enum TRAPS
{
        TRAP_DART,                        /* 0 */
        TRAP_ARROW,                       /* 1 */
        TRAP_SPEAR,                       /* 2 */
        TRAP_AXE,                         /* 3 */
        TRAP_TELEPORT,                    /* 4 */
        TRAP_AMNESIA,                     /* 5 */
        TRAP_BLADE                       /* 6 */
};

enum UNARMED_ATTACKS
{
        UNAT_KICK                      = 1,   /* 1 */
        UNAT_HEADBUTT,                    /* 2 */
        UNAT_TAILSLAP,                    /* 3 */
        UNAT_PUNCH                       /* 4 */
};

enum VORPAL_DESCRIPTIONS
{
        DVORP_CRUSHING,                   /* 0 */
        DVORP_SLICING,                    /* 1 */
        DVORP_PIERCING,                   /* 2 */
        DVORP_CHOPPING                   /* 3 */
};

enum WANDS
{
        WAND_FLAME,                       /* 0 */
        WAND_FROST,                       /* 1 */
        WAND_SLOWING,                     /* 2 */
        WAND_HASTING,                     /* 3 */
        WAND_MAGIC_DARTS,                 /* 4 */
        WAND_HEALING,                     /* 5 */
        WAND_PARALYSIS,                   /* 6 */
        WAND_FIRE,                        /* 7 */
        WAND_COLD,                        /* 8 */
        WAND_CONFUSION,                   /* 9 */
        WAND_INVISIBILITY,                /* 10 */
        WAND_DIGGING,                     /* 11 */
        WAND_FIREBALL,                    /* 12 */
        WAND_TELEPORTATION,               /* 13 */
        WAND_LIGHTNING,                   /* 14 */
        WAND_POLYMORPH_OTHER,             /* 15 */
        WAND_ENSLAVEMENT,                 /* 16 */
        WAND_DRAINING,                    /* 17 */
        WAND_RANDOM_EFFECTS,              /* 18 */
        WAND_DISINTEGRATION              /* 19 */
};

enum WEAPONS
{
        WPN_CLUB,                         /* 0 */
        WPN_MACE,                         /* 1 */
        WPN_FLAIL,                        /* 2 */
        WPN_DAGGER,                       /* 3 */
        WPN_MORNINGSTAR,                  /* 4 */
        WPN_SHORT_SWORD,                  /* 5 */
        WPN_LONG_SWORD,                   /* 6 */
        WPN_GREAT_SWORD,                  /* 7 */
        WPN_SCIMITAR,                     /* 8 */
        WPN_HAND_AXE,                     /* 9 */
        WPN_BATTLEAXE,                    /* 10 */
        WPN_SPEAR,                        /* 11 */
        WPN_HALBERD,                      /* 12 */
        WPN_SLING,                        /* 13 */
        WPN_BOW,                          /* 14 */
        WPN_CROSSBOW,                     /* 15 */
        WPN_HAND_CROSSBOW,                /* 16 */
        WPN_GLAIVE,                       /* 17 */
        WPN_QUARTERSTAFF,                 /* 18 */
        WPN_SCYTHE,                       /* 19 */
        WPN_GIANT_CLUB,                   /* 20 */
        WPN_GIANT_SPIKED_CLUB,            /* 21 */
        WPN_EVENINGSTAR,                  /* 22 */
        WPN_QUICK_BLADE,                  /* 23 */
        WPN_KATANA,                       /* 24 */
        WPN_EXECUTIONERS_AXE,             /* 25 */
        WPN_DOUBLE_SWORD,                 /* 26 */
        WPN_TRIPLE_SWORD,                 /* 27 */
        WPN_HAMMER,                       /* 28 */
        WPN_ANCUS,                        /* 29 */
        WPN_WHIP,                         /* 30 */
        WPN_SABRE,                        /* 31 */
        WPN_DEMON_BLADE,                  /* 32 */
        WPN_DEMON_WHIP                   /* 33 */
};

enum WEAPON_DESCRIPTIONS
{
        DWPN_RUNED                     = 1,   /* 1 */
        DWPN_GLOWING,                     /* 2 */
        DWPN_ORCISH,                      /* 3 */
        DWPN_ELVEN,                       /* 4 */
        DWPN_DWARVEN                     /* 5 */
};

enum WEAPON_PROPERTIES
{
        PWPN_DAMAGE,                      /* 0 */
        PWPN_HIT,                         /* 1 */
        PWPN_SPEED                       /* 2 */
};

enum ZAPS
{
        ZAP_FLAME,                        /* 0 */
        ZAP_FROST,                        /* 1 */
        ZAP_SLOWING,                      /* 2 */
        ZAP_HASTING,                      /* 3 */
        ZAP_MAGIC_DARTS,                  /* 4 */
        ZAP_HEALING,                      /* 5 */
        ZAP_PARALYSIS,                    /* 6 */
        ZAP_FIRE,                         /* 7 */
        ZAP_COLD,                         /* 8 */
        ZAP_CONFUSION,                    /* 9 */
        ZAP_INVISIBILITY,                 /* 10 */
        ZAP_DIGGING,                      /* 11 */
        ZAP_FIREBALL,                     /* 12 */
        ZAP_TELEPORTATION,                /* 13 */
        ZAP_LIGHTNING,                    /* 14 */
        ZAP_POLYMORPH_OTHER,              /* 15 */
        ZAP_VENOM_BOLT,                   /* 16 */
        ZAP_NEGATIVE_ENERGY,              /* 17 */
        ZAP_CRYSTAL_SPEAR,                /* 18 */
        ZAP_BEAM_OF_ENERGY,               /* 19 */
        ZAP_ORB_OF_ENERGY,                /* 20 */
        ZAP_ENSLAVEMENT,                  /* 21 */
        ZAP_AGONY_I,                      /* 22 */
        ZAP_STICKY_FLAME,                 /* 23 */
        ZAP_DISPEL_UNDEAD,                /* 24 */
        ZAP_CLEANSING_FLAME,              /* 25 */
        ZAP_BONE_SHARDS,                  /* 26 */
        ZAP_BANISHMENT,                   /* 27 */
        ZAP_DEGENERATION,                 /* 28 */
        ZAP_STING,                        /* 29 */
        ZAP_HELLFIRE,                     /* 30 */
        ZAP_IRON_BOLT,                    /* 31 */
        ZAP_PEBBLE,                       /* 32 */
        ZAP_STONE_ARROW,                  /* 33 */
        ZAP_ELECTRICITY,                  /* 34 */
        ZAP_ORB_OF_ELECTRICITY,           /* 35 */
        ZAP_SPIT_POISON,                  /* 36 */
        ZAP_DEBUGGING_RAY,                /* 37 */
        ZAP_BREATHE_FIRE,                 /* 38 */
        ZAP_BREATHE_FROST,                /* 39 */
        ZAP_BREATHE_ACID,                 /* 40 */
        ZAP_BREATHE_POISON,               /* 41 */
        ZAP_BREATHE_POWER,                /* 42 */
        ZAP_ENSLAVE_UNDEAD,               /* 43 */
        ZAP_AGONY_II,                     /* 44 */
        ZAP_DISRUPTION,                   /* 45 */
        ZAP_DISINTEGRATION,               /* 46 */
        ZAP_BREATHE_STEAM              = 48,   /* 48 */
        ZAP_CONTROL_DEMON,                /* 49 */
        ZAP_ORB_OF_FRAGMENTATION,         /* 50 */
        ZAP_ICE_BOLT,                     /* 51 */
        ZAP_ICE_STORM                    /* 52 */
};
