#include "externs.h"
#include "stuff.h"

#include <string.h>

char *god_name(int which_god);
char *god_name_long(int which_god);

void pray(void)
{
if (you[0].religion == 0)
{
strcpy(info, "You spend a moment contemplating the meaning of life.");
mpr(info);
return;
}
strcpy(info, "You offer a prayer to ");
strcat(info, god_name(you[0].religion));
strcat(info, ".");
mpr(info);

you[0].duration [3] = 2 + random2(you[0].piety) / 30 + random2(you[0].piety) / 30;

if (you[0].clas == 2 | you[0].clas == 6) you[0].duration [3] = 4 + random2(you[0].piety) / 20 + random2(you[0].piety) / 20 + random2(you[0].piety) / 20;

//strcpy(info, "Zin");

if (you[0].piety <= 20)
{
strcpy(info, "You sense hostility.");
you[0].duration [3] = 0;
} else
if (you[0].piety <= 35)
{
strcat(info, " is most displeased with you.");
you[0].duration [3] = 0;
} else
if (you[0].piety <= 45)
{
strcat(info, " is displeased with you.");
you[0].duration [3] = 0;
} else
if (you[0].piety <= 65)
{
strcat(info, " is noncommittal.");
} else
if (you[0].piety <= 75)
{
strcat(info, " is pleased with you.");
} else
if (you[0].piety <= 90)
{
strcat(info, " is most pleased with you.");
} else
if (you[0].piety <= 110)
{
strcat(info, " favours you.");
you[0].duration [3] *= 2;
} else
if (you[0].piety <= 130)
{
strcat(info, " favours you greatly.");
you[0].duration [3] *= 2;
} else
{
strcat(info, " is exalted by your worship!");
you[0].duration [3] *= 3;
}

mpr(info);

you[0].turnover = 1;

}

char *god_name(int which_god)
{

switch(which_god)
{
/*case 0: strcpy(info, "You aren't religious!"); mpr(info); return;
case 1: strcat(info, "Zin."); break;
case 2: strcat(info, "The Shining One."); break;
case 3: strcat(info, "Elyvilon."); break;
case 4: strcat(info, "Nemelex Xobeh."); break;
case 5: strcat(info, "Jurubetut."); break;
case 6: strcat(info, "Vuhemeti."); break;
case 7: strcat(info, "Okawaru."); break;
case 8: strcat(info, "Kikubaaqudgha."); break;
case 9: strcat(info, "Sif Muna."); break;
case 10: strcat(info, "Lugafu the Hairy."); break;*/
case 0: return "You aren't religious!";
case 1: return "Zin";
case 2: return "The Shining One";
case 3: return "Kikubaaqudgha";
case 4: return "Yredelemnul";
case 5: return "Xom";
case 6: return "Vehumet";
case 7: return "Okawaru";
case 8: return "Makhleb";
case 9: return "Sif Muna";
case 10: return "Lugafu";
case 11: return "Nemelex Xobeh";
case 12: return "Elyvilon";
}
}


char *god_name_long(int which_god)
{

switch(which_god)
{
case 0: return "Nobody";
case 1: return "Zin the Law-Giver";
case 2: return "The Shining One";
case 3: return "Kikubaaqudgha";
case 4: return "Yredelemnul";
case 5: return "Xom of Chaos";
case 6: return "Vehumet";
case 7: return "Okawaru";
case 8: return "Makhleb";
case 9: return "Sif Muna";
case 10: return "Lugafu the Hairy";
case 11: return "Nemelex the Trickster";
case 12: return "Elyvilon the Healer";
}
}
