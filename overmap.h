enum
{
    NO_FEATURE = 0,             // clear
     FEATURE_SHOP = 1,          // 1 << 0
     FEATURE_LABYRINTH = 2,     // (1<<1)
     FEATURE_HELL = 4,          // (1<<2)
     FEATURE_ABYSS = 8,         // (1<<3)
     FEATURE_PANDEMONIUM = 16   //  (1<<4)
};

void seen_altar(unsigned char which_altar);
bool print_level_name(int i, int j, bool already_printed);
void init_overmap(void);
void display_overmap(void);
void seen_staircase(unsigned char which_staircase);
void seen_other_thing(unsigned char which_thing);
