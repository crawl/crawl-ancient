#include <conio.h>
//#include <alloc.h>
#include <string.h>
#include <stdlib.h>


#include <iostream.h>// Only for debugging!

#include "fn3.h"
#include "builder2.h"
//#include "newname.h"

#include <process.h>


#define MNST 200
#define ITEMS 500

char id [4] [30]; // should this be static?

int random2(unsigned int rmax);

char store_message [78] [78];
char store_count = 0;




//#include <alloc.h>

char is_a_vowel(unsigned char let);
char retvow(char sed);
char retbit(char sed);
char reduce(unsigned char reducee);

char *retcons(char seeds);

char *make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase);


char spell_value(char spell);

void spell_name(char spell, char spln [60]);

char spellbook_contents(unsigned char plus, unsigned char type);

void spellbook_template(int sbook_type, int func_pass [10]);



char *make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase)
//(char item_clas, char item_typ, unsigned char item_da, char it_plus, unsigned int it_quant, char ident_lev)
//int main(void)
{

//char *glag;
char name [30] = "";// = "333333333";
char tmp_quant [5];
unsigned char numb [15];
char len;
char i = 0;
char nexty = 0;
char j = 0;
char igo = 0;

int ix = 0;

//char outp [30];

//strcpy(outp, "                                    ");

//randomize();

//beginn : char var1 = getch();
//char var2 = getch();
//char var3 = getch();

//beginn : //char var1 = random(26);
//char var2 = random(26);
//char var3 = random(26);

//glag = (char *)malloc(30);

char glag [30];
/*
if (!glag)
{
        cprintf("Not enough memory to allocate buffer!");
        exit(0);
}*/

strcpy(glag, "");


numb [0] = var1 * var2;
numb [1] = var1 * var3;
numb [2] = var2 * var3;
numb [3] = var1 * var2 * var3;
numb [4] = var1 + var2;
numb [5] = var2 + var3;
numb [6] = var1 * var2 + var3;
numb [7] = var1 * var3 + var2;
numb [8] = var2 * var3 + var1 ;
numb [9] = var1 * var2 * var3 - var1;
numb [10] = var1 + var2 + var2;
numb [11] = var2 + var3 * var1;
numb [12] = var1 * var2 * var3;
numb [13] = var1 * var3 * var1;
numb [14] = var2 * var3 * var3;
//numb [15] = var1 * var2 * var3;
//numb [16] = var1 + var2;
//numb [17] = var2 + var3;





for (i = 0; i < 15; i ++)
{
        while(numb [i] >= 25)
        {
                numb [i] -= 25;
        }
} // end of for i

j = numb [6];

//len = (numb[9] + numb [10] + numb [5] + numb [3]) / 6;
len = reduce(numb [reduce(numb [11]) / 2]);
while (len < 5 && j < 10) //len += 5;
{
        len += reduce(numb [j] + 1) + 1;
        j++;
}

while (len > 14)
{
        len -= 8;
}

//len = 8;

//if (len > 18) len -= 10;

nexty = retbit(numb [4]);
char k = 0;
j = 0;

for (i = 0; i < len; i ++)
{
j ++;
if (j >= 15)
{
        j = 0;
        k++;
        if (k > 9) break;
}

if (nexty == 1 | (i > 0 && is_a_vowel(name [i]) == 0))
{
        name [i] = retvow(numb [j]);
        if ((i == 0 | i == len - 1) && name [i] == 32)
        {
                i--;
                continue;
        }
} else
{
        if (numb [i / 2] <= 1 && i > 3 && is_a_vowel(name [i]) == 1) goto two_letter;
             else
             name [i] = numb [j];//retcons(numb [j]);
        //strcpy(name, retcons(numb [j]));
//      name [i] = numb [j];
//           strcat(name, "XX");
        hello : igo ++;
}

if ((nexty == 0 && is_a_vowel(name [i]) == 1) | (nexty == 1 && is_a_vowel(name [i]) == 0))
{
        if (nexty == 1 && i > 0 && is_a_vowel(name [i - 1]) == 0) i --;
        i--;
        //j++;
        //k++;
//      break;
        continue;
}


if (is_a_vowel(name [i]) == 0)//nexty == 0)
{
        //if (retbit(numb [1]) | retbit(numb [3]) | retbit(numb [5]) | retbit(numb [7]) | retbit(numb [10])) nexty = 1;
        //if (retbit(numb [j]) | retbit(numb [i]))
        nexty = 1;
} else nexty = 0;

ix++;

}
/*
for (i = 0; i < len; i++)
{
        if (name [i] > 96) name [i] -= 97;
}
*/

switch(ncase)
{
case 2:
for (i = 0; i < len + 1; i ++)
{
        if (i > 3 && name [i] == 0 && name [i + 1] == 0)
        {
                name [i] = 0;
                if (name [i - 1] == 32) name [i - 1] = 0;
                break;
        }
        //break;
        if (name [i] != 32 && name [i] < 30)
                name [i] += 65; //97;
        if (name [i] > 96) name [i] -= 32;
        //if (name [i] < 65 | name [i] > 90) name [i] = 85;
}
//name [i + 1] = 0;
break;

case 0:
for (i = 0; i < len; i ++)
{
        if (name [i] != 32 && name [i] < 30)
                name [i] += 97;
}
break;

case 1:
name [i] += 65;
for (i = 1; i < len; i ++)
{
        if (name [i] != 32 && name [i] < 30)
                name [i] += 97; //97;
}
break;
}
//clrscr();

strcpy(glag, name);


//strncpy(outp, name, len);

//outp [len] = '\0';

//cprintf(outp);
//cprintf(name);

//cprintf("\n\r");

//i = getch();

//goto beginn;


/*
if (ncase == 2)
{
        for (i = 0; i < 30; i ++)
        {
                if
        }
}
*/

//free(glag);

return glag;




two_letter : //goto hello;
//strcat(name, "xx");
//goto hello;
if (nexty == 1) goto hello;

if (!is_a_vowel(name [i - 1])) goto hello;

i ++;
switch(i * (retbit(j) + 1))
{
case 0: strcat(name, "sh"); break;
case 1: strcat(name, "ch"); break;
case 2: strcat(name, "tz"); break;
case 3: strcat(name, "ts"); break;
case 4: strcat(name, "cs"); break;
case 5: strcat(name, "cz"); break;
case 6: strcat(name, "xt"); break;
case 7: strcat(name, "xk"); break;
case 8: strcat(name, "kl"); break;
case 9: strcat(name, "cl"); break;
case 10: strcat(name, "fr"); break;
case 11: strcat(name, "sh"); break;
case 12: strcat(name, "ch"); break;
case 13: strcat(name, "gh"); break;
case 14: strcat(name, "pr"); break;
case 15: strcat(name, "tr"); break;
case 16: strcat(name, "mn"); break;
case 17: strcat(name, "ph"); break;
case 18: strcat(name, "pn"); break;
case 19: strcat(name, "cv"); break;
case 20: strcat(name, "zx"); break;
case 21: strcat(name, "xz"); break;
case 23: strcat(name, "dd"); break;
case 24: strcat(name, "tt"); break;
case 25: strcat(name, "ll"); break;
//case 26: strcat(name, "sh"); break;
//case 12: strcat(name, "sh"); break;
//case 13: strcat(name, "sh"); break;
default:
//strcat(name, "xx");
i--;
//name [i - 1] -= 97;
goto hello;
//break;
}
//name [i] -= 97;
//name [i - 1] -= 97;

ix += 2;

goto hello;

//return 0;

} // end of make_name



char reduce(unsigned char reducee)
{

while(reducee >= 26)
{
        reducee -= 26;
}

return reducee;

} // end of char reduce



char is_a_vowel(unsigned char let)
{
        //if (let == 'a' | let == 'e' | let == 'i' | let == 'o' | let == 'u')
        if (let == 0 | let == 4 | let == 8 | let == 14 | let == 20 | let == 24 | let == 32)
        {
                return 1;
        } else return 0;
} // end of char reduce

/*
char *retcons(char seeds)
{

char two [2] = "";

//char *two;

//two = (char *)malloc(2);

//if (!two) cprintf("Not enough memory to allocate buffer!");

//strcpy(two, "");



//char two [2] = "";

//strcpy(two, "");


two [0] = reduce(seeds);

//free(two);

return two;

}
*/




char retvow(char sed)
{

//if (sed >= 20) return 24;

//if (sed >= 18) return 32;

while(sed > 6)
{
        sed -= 6;
}

switch(sed)
{
        case 0: return 0;
        case 1: return 4;
        case 2: return 8;
        case 3: return 14;
        case 4: return 20;
        case 5: return 24;
        case 6: return 32;
}

return 0;
}


char retbit(char sed)
{
        /*while(sed > 1)
        {
                sed -= 2;
        }*/

        if (sed % 2 == 0) return 0;

        return 1;

}




//Make all scrolls of a certain type in one dungeon be labelled same - have a random number which is put into item_plus for all scrolls.




void save_id(char identy [4] [30])
{

char ix = 0;
char jx = 0;

for (ix = 0; ix < 4; ix ++)
{
 for (jx = 0; jx < 30; jx ++)
 {
  identy [ix] [jx] = id [ix] [jx];
 }
}

}









// wands, scrolls, rings, potions


//static int inflin;
//static int binflin;

void initial(void)
{
char i = 0;
char j = 0;

//inflin = 0;
//binflin = 0;

for (i = 0; i < 4; i ++)
{
        for (j = 0; j < 30; j ++)
        {
                id [i] [j] = 0;
        }
}

} // end of void initial

/*

char incrline(void)
{

inflin++;

return inflin;

}

void zeroinf(void)
{

inflin = 0;

}
*/


void set_id(char cla, char ty, char setting)
{
        if (cla > 99)
        {
                cla -= 100;
                id [cla] [ty] = setting;
                return;
        }

        switch(cla)
        {
        case 3: id [0] [ty] = setting; break;
        case 6: id [1] [ty] = setting; break;
        case 7: id [2] [ty] = setting; break;
        case 8: id [3] [ty] = setting; break;
        // if it's none of these, will just return
        }
} // end of void set_id


char get_id(char cla, char ty)
{
        if (cla > 99)
        {
                cla -= 100;
                return id [cla] [ty];
        }

        switch(cla)
        {
        case 3: return id [0] [ty];
        case 6: return id [1] [ty];
        case 7: return id [2] [ty];
        case 8: return id [3] [ty];
        // if it's none of these, will just return
        }
return 0;

} // end of char get_id





char message(char inf [78] [78], int inf_lines, char scrloc)
{
int over_check = 0;// stops the printing of too many lines.

char jg = 0;
char inf_screens = 0;
int i = 0;
//static char store_message [78] [78];

window(2, 18, 78, 25);
//clrscr();

if (inf_lines == 0) return;

//inf_screens = inf_lines / 8 + 1;
textcolor(7);

if (scrloc == 8)
{
 more();
 clrscr();
 scrloc = 1;
}

gotoxy(1, scrloc);
cprintf(inf [0]);

strcpy(store_message [store_count], inf [0]);
store_count ++;
if (store_count > 23) store_count = 0;

strcpy(store_message [store_count], "------------------------------------------------------------------------------");

//for (i = 0; i < 79; i ++)
//{
//      store_message [store_count] [i] = 196;
//}

/*
for (me_c = 0; me_c < inf_screens; me_c++)
{
        if (inf_lines - me_c * 8 > 8) over_check = 8;
                else over_check = inf_lines - me_c * 8;


                for (jg = 0; jg < over_check; jg++)
                {
                        gotoxy(1, jg + 1);
                        cprintf(inf [jg + me_c * 8]);
                        textcolor(7); textbackground(0);
                }
        if (inf_screens > me_c + 1 && inf_lines - (me_c + 1) * 8 != 0)// && inf_screens != 1)
        {
                gotoxy(1 , 8);
                more();
                clrscr();
        }
//if (me_c = 5)
} // end of me_c loop.
*/

inf_screens = 0;
//inf_lines = 0;

return scrloc;

} // end of message function





void replay_messages(void)
{

//   char st_pass [60];

   int i;
   char buffer[4800];
   window(1, 1, 80, 25);
   gettext(1, 1, 80, 25, buffer);

   clrscr();

   gotoxy(1,1);

                        for (i = 0; i < 24; i ++)
                        {
    cprintf(store_message [i]);
                                cprintf("\n\r");
                        }

   getch();


   putty : puttext(1, 1, 80, 25, buffer);

   return;

}







/*
void message(char inf [78] [78], int inf_lines)
{
int over_check = 0;// stops the printing of too many lines.
int me_c; // loop variable - after all, lots of functions call this one.
//char ig = 0;
char jg = 0;
char inf_screens = 0;

//char n_lines = inflin;

window(2, 16, 78, 24);
clrscr();
//inf_lines--;
if (inf_lines == 0) return;

inf_screens = inf_lines / 8 + 1;
textcolor (7);

for (me_c = 0; me_c < inf_screens; me_c++)
{
        if (inf_lines - me_c * 8 > 8) over_check = 8;
                else over_check = inf_lines - me_c * 8;


                for (jg = 0; jg < over_check; jg++)
                {
                        gotoxy(1, jg + 1);
                        //if (inf [jg + me_c * 8] [0] == 95) // reversing
                        //{
                        //      textcolor(0); textbackground(7);
                        //      inf [jg + me_c * 8] [0] = 32;
                        //}
                        cprintf(inf [jg + me_c * 8]);
                        textcolor(7); textbackground(0);
                }
        if (inf_screens > me_c + 1 && inf_lines - (me_c + 1) * 8 != 0)// && inf_screens != 1)
        {
                gotoxy(1 , 8);
                more();
                clrscr();
        }
//if (me_c = 5)
} // end of me_c loop.

//binflin = inflin;

inf_screens = 0;
inf_lines = 0;
//line_no++;
//       if (line_no >9)
//       {
//       clrscr();
//       line_no = 9;
//       }
//if (keyin == 59) more();
//char *fdg;
//fdg = 'h';

//return fdg;

} // end of message function
*/








char item_name_2(char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog [60])
{

//char *glog;
//char gmon_name [60] = "";
char tmp_quant [5];


//glog = (char *)malloc(60);

//if (!glog) cprintf("Not enough memory to allocate buffer!");

strcpy(glog, "");

// Put in cursed??


// Look at: item_plus is unsigned, so what happens when a -ve weapon is
// uncursed?


switch(item_clas)
        {

        case 0: // weapons


        if (ident_lev > 0)
        {

        if (it_plus >= 130)
        {
                strcat(glog, "cursed ");
        }


        }


        if (ident_lev > 2)
        {
        /*
                int hogh = it_plus - 50;
                int tohit = 0;
                int todam = 0;

                if (hogh > 80)
                {
                        hogh -= 100;
                        itoa(hogh, tmp_quant, 10);
                        strcat(glog, tmp_quant);
                } else
                {
                //hogh -= 100;
                tohit = hogh / 10;
                todam = hogh % 10;
                if (tohit == todam)
                {
                        itoa(tohit, tmp_quant, 10);
                        strcat(glog, "+");
                        strcat(glog, tmp_quant);
                        strcat(glog, " ");
                } else
                      {
                        itoa(tohit, tmp_quant, 10);
                        strcat(glog, "+");
                        strcat(glog, tmp_quant);
                        strcat(glog, "/");
                        strcat(glog, "+");
                        itoa(todam, tmp_quant, 10);
                        strcat(glog, tmp_quant);
                        strcat(glog, " ");
                      }
                } // end of else
          */


        if (it_plus >= 50 && (it_plus <= 130 | it_plus >= 150)) strcat(glog, "+");
        itoa(it_plus - 50, tmp_quant, 10);
        if (it_plus > 130) itoa((it_plus - 150), tmp_quant, 10);
        strcat(glog, tmp_quant);
        strcat(glog, " ");

//      if (item_da >= 50 && (item_da <= 130 | item_da >= 150)) strcat(glog, "+");
//      itoa(item_da - 50, tmp_quant, 10);
//      if (it_plus > 130) itoa((it_plus - 150), tmp_quant, 10);
// itoa(item_da, tmp_quant, 10);
//      strcat(glog, tmp_quant);
//      strcat(glog, " ");


        }

 if (item_da > 180)
 {

  if (ident_lev > 0)
  {
   switch(item_da - 180)
   {
        case 1: strcat(glog , "Singing Sword"); break;
        case 2: strcat(glog , "Wrath of Trog"); break;
        case 3: strcat(glog , "Scythe of Curses"); break;
        case 4: strcat(glog , "Mace of Variability"); break;
        case 5: strcat(glog , "Glaive of Prune"); break;
        case 6: strcat(glog , "S"); break;
        case 7: strcat(glog , ""); break;
        case 8: strcat(glog , ""); break;
        case 9: strcat(glog , ""); break;

   }
  break;
  } // end if ident_lev
   switch(item_da - 180)
   {
        case 1: strcat(glog , "golden long sword"); break;
        case 2: strcat(glog , "bloodstained battleaxe"); break;
        case 3: strcat(glog , "warped scythe"); break;
        case 4: strcat(glog , "shimmering mace"); break;
        case 5: strcat(glog , "purple glaive"); break;
        case 6: strcat(glog , ""); break;
        case 7: strcat(glog , ""); break;
        case 8: strcat(glog , ""); break;
        case 9: strcat(glog , ""); break;

   }
 break;
 } // end uniques


//if (ident_lev <= 2 | item_da % 30 == 0)
if (item_da % 30 == 0)
{
switch(item_da / 30)
{
   case 1:
   if (item_typ == 0 | (item_typ <= 18 && item_typ >= 13))
   strcat(glog, ""); else
                   strcat(glog, "shiny "); break;
   case 2: strcat(glog, "ancient "); break;
   case 3: strcat(glog, "glowing "); break;
   case 4: strcat(glog, "ornate "); break;
   case 5: strcat(glog, "runed "); break;
//   case 6: strcat(glog, ""); break;
} // end switch
} // end if


        switch(item_typ)
        {
        case 0: strcat(glog , "club"); break;
        case 1: strcat(glog , "mace"); break;
        case 2: strcat(glog , "flail"); break;
        case 3: strcat(glog , "dagger"); break;
        case 4: strcat(glog , "spiked mace"); break;
        case 5: strcat(glog , "short sword"); break;
        case 6: strcat(glog , "long sword"); break;
        case 7: strcat(glog , "great sword"); break;
        case 8: strcat(glog , "scimitar"); break;
        case 9: strcat(glog , "hand axe"); break;
        case 10: strcat(glog , "battleaxe"); break;
        case 11: strcat(glog , "spear"); break;
        case 12: strcat(glog , "halberd"); break;
        case 13: strcat(glog , "sling"); break;
        case 14: strcat(glog , "bow"); break;
        case 15: strcat(glog , "crossbow"); break;
 case 16: strcat(glog , "hand crossbow"); break;
 case 17: strcat(glog , "glaive"); break;
        case 18: strcat(glog , "quarterstaff"); break;
 case 19: strcat(glog , "scythe"); break;
        case 20: strcat(glog , "giant club"); break;
        case 21: strcat(glog , "giant spiked club"); break;

        }

if (ident_lev > 1)
{
 switch(item_da % 30)
 {
  case 0: break;
  case 1: strcat(glog, " of flaming"); break;
  case 2: strcat(glog, " of freezing"); break;
  case 3: strcat(glog, " of holy wrath"); break;
  case 4: strcat(glog, " of electrocution"); break;
  case 5: strcat(glog, " of orc slaying"); break;
  case 6: strcat(glog, " of venom"); break;
  case 7: strcat(glog, " of protection"); break;
  case 8: strcat(glog, " of draining"); break;
 }
}

// Hellbrand; zap to reduce plus by 1 and cast hellfire?

 break;


        case 1: // ammunition
        if (ident_lev > 0)
        {
                unsigned char gokh = it_plus;
                //it_plus -= 50;
                if (gokh >= 50 && (gokh <= 130 | gokh >= 150)) strcat(glog, "+");
                itoa(gokh - 50, tmp_quant, 10);
                if (gokh > 130) itoa((gokh - 150), tmp_quant, 10);
                strcat(glog, tmp_quant);
                strcat(glog, " ");
        }

        switch(item_typ)
        {
        case 0: strcat(glog , "stone"); break;
        case 1: strcat(glog , "arrow"); break;
        case 2: strcat(glog , "bolt"); break;
        case 3: strcat(glog , "dart"); break;
        case 4: strcat(glog , "eggplant"); break;
        case 5: strcat(glog , "large rock"); break;
        case 6: strcat(glog , ""); break;
        case 7: strcat(glog , ""); break;
        case 8: strcat(glog , ""); break;
        case 9: strcat(glog , ""); break;
        case 10: strcat(glog , ""); break;
        case 12: strcat(glog , ""); break;
        case 13: strcat(glog , ""); break;
        case 14: strcat(glog , ""); break;
        case 15: strcat(glog , ""); break;
        case 16: strcat(glog , ""); break;
        } break;

        //break;



        case 2: // armour


        if (ident_lev > 0)
        {
                if (it_plus >= 130)
                {
                        strcat(glog, "cursed ");
                }
        }




        if (ident_lev > 2)
        {
                if (it_plus >= 50 && (it_plus <= 130 | it_plus >= 150)) strcat(glog, "+");
                itoa(it_plus - 50, tmp_quant, 10);
                if (it_plus > 130) itoa((it_plus - 150), tmp_quant, 10);
                strcat(glog, tmp_quant);
                //itoa(it_plus, tmp_quant, 10);
                //strcat(glog, tmp_quant);
                strcat(glog, " ");
        }

if (item_typ == 11 | item_typ == 12) strcat(glog, "pair of ");

if (ident_lev < 2 | item_da % 30 == 0)
{
if (item_typ != 10)
{
 switch(item_da / 30)
 {
   case 1:
   if (item_typ == 0 | item_typ == 9 | item_typ == 11 | item_typ == 12)
   strcat(glog, "embroidered "); else
                if (item_typ == 1) strcat(glog, ""); else
                   strcat(glog, "shiny "); break;
   case 2: strcat(glog, "ancient "); break;
   case 3: strcat(glog, "glowing "); break;
   case 4: strcat(glog, "ornate "); break;
   case 5: strcat(glog, "runed "); break;
 } // end switch
}
if (item_typ == 10)// && item_da >= 6)
{
switch(item_da / 30)
{
   case 1: strcat(glog, "winged "); break;
   case 2: strcat(glog, "horned "); break;
   case 3: strcat(glog, "crested "); break;
   case 4: strcat(glog, "plumed "); break;
   case 5: strcat(glog, "spiked "); break;
   case 6: strcat(glog, "visored "); break;
   case 7: strcat(glog, "jewelled "); break;
   case 13: strcat(glog, " "); break;
   case 14: strcat(glog, " "); break;
   case 15: strcat(glog, " "); break;
}
}

} // end if


        switch(item_typ )
        {
        case 0: strcat(glog , "robe"); break;
//      icolour  = random(15) + 1;
        case 1: strcat(glog , "leather armour");
//      icolour  = 6;
        break;
        case 2: strcat(glog , "ring mail"); break;
        case 3: strcat(glog , "scale mail"); break;
        case 4: strcat(glog , "chain mail"); break;
        case 5: strcat(glog , "splint mail"); break;
        case 6: strcat(glog , "banded mail"); break;
        case 7: strcat(glog , "plate mail"); break;
        case 8: strcat(glog , "shield"); break;
        case 9: strcat(glog , "cloak");
//      icolour  = random(15) + 1;
        break;
        case 10: strcat(glog , "helmet"); break;
        case 11: strcat(glog , "gloves");
//      icolour  = 6;
        break;
        case 12: strcat(glog , "boots");
//      icolour  = 6;
        break;
        case 13: strcat(glog , "buckler"); break;
        case 14: strcat(glog , "large shield"); break;
        }

 if (ident_lev > 1)
 {
  switch(item_da % 30)
  {
   case 1: strcat(glog, " of running"); break;
   case 2: strcat(glog, " of fire resistance"); break;
   case 3: strcat(glog, " of cold resistance"); break;
   case 4: strcat(glog, " of poison resistance"); break;
   case 5: strcat(glog, " of see invisible"); break;
   case 6: strcat(glog, " of invisibility"); break;
   case 7: strcat(glog, " of strength"); break;
   case 8: strcat(glog, " of dexterity"); break;
   case 9: strcat(glog, " of intelligence"); break;
  case 10: strcat(glog, " of ponderousness"); break;
  case 11: strcat(glog, " of levitation"); break;
  case 12: strcat(glog, " of magic resistance"); break;
  }
 }

 break;


        case 3: // devices
        //if (ident_lev > 0)
        if (id [0] [item_typ] == 1)
        {
                switch(item_typ)
                {
                case 0: strcat(glog, "wand of flame"); break;
                case 1: strcat(glog, "wand of frost"); break;
                case 2: strcat(glog, "wand of slowing"); break;
                case 3: strcat(glog, "wand of hasting"); break;
                case 4: strcat(glog, "wand of magic darts"); break;
                case 5: strcat(glog, "wand of healing"); break;
                case 6: strcat(glog, "wand of paralysis"); break;
                case 7: strcat(glog, "wand of fire"); break;
                case 8: strcat(glog, "wand of cold"); break;
                case 9: strcat(glog, "wand of confusion"); break;
                case 10: strcat(glog, "wand of invisibility"); break;
                case 11: strcat(glog, "wand of digging"); break;
      case 12: strcat(glog, "wand of fireball"); break;
      case 13: strcat(glog, "wand of teleportation"); break;
                case 14: strcat(glog, "wand of lightning"); break;
      case 15: strcat(glog, "wand of polymorph"); break;
      case 16: strcat(glog, "wand of enslavement"); break;
      } //break;
        } else
        {
        if (item_da <= 11)
        {
//      randnum = random(12);
        switch(item_da )
        {
        case 0: strcat(glog , "iron wand"); break;
        case 1: strcat(glog , "brass wand"); break;
        case 2: strcat(glog , "bone wand"); break;
        case 3: strcat(glog , "wooden wand"); break;
        case 4: strcat(glog , "copper wand"); break;
        case 5: strcat(glog , "gold wand"); break;
        case 6: strcat(glog , "silver wand"); break;
        case 7: strcat(glog , "bronze wand"); break;
        case 8: strcat(glog , "ivory wand"); break;
        case 9: strcat(glog , "glass wand"); break;
        case 10: strcat(glog , "lead wand"); break;
        case 11: strcat(glog , "plastic wand"); break;
/*      case 12: strcat(glog , " wand");
        icolour  = 6;  break;
        case 13: strcat(glog , " wand");
        icolour  = 14; break;
        case 14: strcat(glog , " wand");
        icolour  = 7;  break;
        case 15: strcat(glog , " wand");
        icolour  = 7; break; */

        } // end of switch(randnum)

        } else //ie if it's uncommon:
        {
//      secchar = random(16);
//      secchar = item_da  % 16;
        switch(item_da / 12) // secondary characteristic of uncommon wand
        {

//      case 0: strcat(glog , "blackened "); break;
///     DON'T WANT IT HERE ^^^^^^

        case 1: strcat(glog , "jewelled "); break;
        case 2: strcat(glog , "curved "); break;
        case 3: strcat(glog , "long "); break;
        case 4: strcat(glog , "short "); break;
        case 5: strcat(glog , "twisted "); break;
        case 6: strcat(glog , "crooked "); break;
        case 7: strcat(glog , "forked "); break;
        case 8: strcat(glog , "shiny "); break;
        case 9: strcat(glog , "blackened "); break;
        case 10: strcat(glog , "tapered "); break;
        case 11: strcat(glog , "glowing "); break;
        case 12: strcat(glog , "worn "); break;
        case 13: strcat(glog , "encrusted "); break;
        case 14: strcat(glog , "runed "); break;
        case 15: strcat(glog , "sharpened "); break;
        } // end of switch(randnum)


//      randnum = random(10);
//        randnum = item_da  / 12;
        switch((int) item_da % 12)
        {
        case 0: strcat(glog , "iron wand"); break;
        case 1: strcat(glog , "brass wand"); break;
        case 2: strcat(glog , "bone wand"); break;
        case 3: strcat(glog , "wooden wand"); break;
        case 4: strcat(glog , "copper wand"); break;
        case 5: strcat(glog , "gold wand"); break;
        case 6: strcat(glog , "silver wand"); break;
        case 7: strcat(glog , "bronze wand"); break;
        case 8: strcat(glog , "ivory wand"); break;
        case 9: strcat(glog , "glass wand"); break;
        case 10: strcat(glog , "lead wand"); break;
        case 11: strcat(glog , "plastic wand"); break;
        } // end of switch(randnum)

        } // end of if uncommon wand

        } // end of else for ident_lev

        // rare wands (eg disintegration - these will be very rare): maybe only 1 thing, like: crystal, shining, etc.
        if (ident_lev > 1)
        {
                strcat(glog, " (");
//              if (it_plus >= 0 && (plus <= 80 | plus >= 100)) strcat(glog, "+");
                itoa(it_plus, tmp_quant, 10);
                if (it_plus > 80) itoa((it_plus - 100), tmp_quant, 10);
                strcat(glog, tmp_quant);
                strcat(glog, ")");
        }
        break;




        case 8: // potions

        /*if (it_quant == 1)
        {
                strcat(glog, "bottle of ");
        } else
        {
                strcat(glog, "bottles of ");
        }*/

        if (id [3] [item_typ] == 1)
        {
                if (it_quant == 1)
                {
                        strcat(glog, "potion of ");
                } else
                {
                        strcat(glog, "potions of ");
                }
        }

        //if (ident_lev > 0)
        if (id [3] [item_typ] == 1)
        {
                switch(item_typ)
                {
                case 0: strcat(glog , "healing"); break;
                case 1: strcat(glog , "heal wounds"); break;
                case 2: strcat(glog , "speed"); break;
                case 3: strcat(glog , "might"); break;
                case 4: strcat(glog , "gain strength"); break;
                case 5: strcat(glog , "gain dexterity"); break;
                case 6: strcat(glog , "gain intelligence"); break;
                case 7: strcat(glog , "levitation"); break;
                case 8: strcat(glog , "poison"); break;
                case 9: strcat(glog , "slowing"); break;
                case 10: strcat(glog , "paralysis"); break;
                case 11: strcat(glog , "confusion"); break;
                case 12: strcat(glog , "invisibility"); break;
                case 13: strcat(glog , "porridge"); break; // always gluggy?case 7: strcat(glog , "poison"); break;
                case 14: strcat(glog , "disease"); break;
                case 15: strcat(glog , "decay"); break; // Rare!
  case 16: strcat(glog , "water"); break;
  case 17: strcat(glog , "experience"); break;
  case 18: strcat(glog , "magic"); break;
  case 19: strcat(glog , "restore abilities"); break;
                case 20: strcat(glog , "strong poison"); break;
  }
        break;
        } // end of if ident_lev

//      if (item_typ  < 4)
 if (item_da <= 13)
        {
//      randnum = random(14);
        switch(item_da )
        {
                case 0: strcat(glog , "clear potion"); break;
                case 1: strcat(glog , "blue potion"); break;
                case 2: strcat(glog , "black potion"); break;
                case 3: strcat(glog , "silvery potion"); break;
                case 4: strcat(glog , "cyan potion"); break;
                case 5: strcat(glog , "purple potion"); break;
                case 6: strcat(glog , "orange potion"); break;
                case 7: strcat(glog , "inky potion"); break;
                case 8: strcat(glog , "red potion"); break;
                case 9: strcat(glog , "yellow potion"); break;
                case 10: strcat(glog , "green potion"); break;
                case 11: strcat(glog , "brown potion"); break;
                case 12: strcat(glog , "pink potion"); break;
                case 13: strcat(glog , "white potion"); break;
/*              case 14: strcat(glog , "potion");
                icolour  = 7;  break;
                case 15: strcat(glog , "potion");
                icolour  = 7; break;*/

        } // end of switch(randnum)

        } else //ie if it's uncommon:
        {
//      secchar = random(13);
        switch(item_da / 14) // secondary characteristic of uncommon wand
        {
                case 0: strcat(glog , "bubbling "); break;
                case 1: strcat(glog , "lumpy "); break;
                case 2: strcat(glog , "fuming "); break;
                case 3: strcat(glog , "smoky "); break;
                case 4: strcat(glog , "fizzy "); break;
                case 5: strcat(glog , "glowing "); break;
                case 6: strcat(glog , "sedimented "); break;
                case 7: strcat(glog , "metallic "); break;
                case 8: strcat(glog , "murky "); break;
                case 9: strcat(glog , "gluggy "); break;
                case 10: strcat(glog , "viscous "); break;
                case 11: strcat(glog , "oily "); break;
                case 12: strcat(glog , "slimy "); break;
/*              case 13: strcat(glog , " ");
                icolour  = 14; break;
                case 14: strcat(glog , " ");
                icolour  = 7;  break;
                case 15: strcat(glog , " ");
                icolour  = 7;  break;*/

        } // end of switch(randnum)


//      randnum = random(14);
        switch(item_da % 14)
        {
                case 0: strcat(glog , "clear potion"); break;
                case 1: strcat(glog , "blue potion"); break;
                case 2: strcat(glog , "black potion"); break;
                case 3: strcat(glog , "silvery potion"); break;
                case 4: strcat(glog , "cyan potion"); break;
                case 5: strcat(glog , "purple potion"); break;
                case 6: strcat(glog , "orange potion"); break;
                case 7: strcat(glog , "inky potion"); break;
                case 8: strcat(glog , "red potion"); break;
                case 9: strcat(glog , "yellow potion"); break;
                case 10: strcat(glog , "green potion"); break;
                case 11: strcat(glog , "brown potion"); break;
                case 12: strcat(glog , "pink potion"); break;
                case 13: strcat(glog , "white potion"); break;
/*              case 14: strcat(glog , "potion");
                icolour  = 7;  break;
                case 15: strcat(glog , "potion");
                icolour  = 7; break;*/

        } // end of switch(randnum)
        } // end of else.
   if (it_quant > 1) strcat(glog, "s");
        break;

        case 4: // food
//      icolour  = 6;
        switch(item_typ )
        {
        case 0: strcat(glog , "meat ration"); break;
        case 1: strcat(glog , "bread ration"); break;
        case 2: strcat(glog , "pear"); break;
        case 3: strcat(glog , "apple"); break; // make this less common.
        case 4: strcat(glog , "choko"); break;
        case 5: strcat(glog , "honeycomb"); break;
        case 6: strcat(glog , "royal jell"); break; // maybe a joke monster of the same name? - mix something with jelly crystals?
        case 7: strcat(glog , "snozzcumber"); break;
 case 8: strcat(glog , "slice of pizza"); break;
 case 9: strcat(glog , "apricot"); break;
 case 10: strcat(glog , "orange"); break;
 case 11: strcat(glog , "banana"); break;
 case 12: strcat(glog , "strawberr"); break;
 case 13: strcat(glog , "rambutan"); break;
 case 14: strcat(glog , "lemon"); break;
 case 15: strcat(glog , "grape"); break;
 case 16: strcat(glog , "sultana"); break;
 case 17: strcat(glog , "lychee"); break;
 case 18: strcat(glog , "beef jerk"); break;
 case 19: strcat(glog , "cheese"); break;
 case 20: strcat(glog , "sausage"); break;
 case 21: strcat(glog , ""); break;
        }

 if (item_typ == 6 | item_typ == 12 | item_typ == 18)
        {
  if (it_quant > 1)
  {
   strcat(glog, "ie");
  } else strcat(glog, "y");
        }
 break;

        case 6: // scrolls
//      icolour  = 7;

        if (it_quant == 1)
        {
                strcat(glog, "scroll ");
        } else
        {
                strcat(glog, "scrolls ");
        }


        // Scrolls need to be looked at!!!

        if (id [1] [item_typ] == 1)
        {

        switch(item_typ)
        {
        case 0: strcat(glog , "of identify"); break;
        case 1: strcat(glog , "of teleportation"); break;
        case 2: strcat(glog , "of fear"); break;
        case 3: strcat(glog , "of noise"); break;
        case 4: strcat(glog , "of remove curse"); break;
        case 5: strcat(glog , "of detect curse"); break;
        case 6: strcat(glog , "of summoning"); break;
        case 7: strcat(glog , "of enchant weapon"); break;
        case 8: strcat(glog , "of enchant armour"); break;
        case 9: strcat(glog , "of torment"); break;
        case 10: strcat(glog , "of random uselessness"); break;
        case 11: strcat(glog , "of curse weapon"); break;
        case 12: strcat(glog , "of curse armour"); break;
        case 13: strcat(glog , "of immolation"); break;
        case 14: strcat(glog , "of blinking"); break;
 case 15: strcat(glog , "of paper"); break;
        case 16: strcat(glog , "of magic mapping"); break;
        case 17: strcat(glog , "of forgetfulness"); break;
        case 18: strcat(glog , ""); break;
        } break;

        }
        /*
        switch(item_da)
        {
        case 0: strcat(glog , "labelled HELLO"); break;
        case 1: strcat(glog , "labelled READ ME"); break;
        case 2: strcat(glog , "labelled ZXZXZX"); break;
        case 3: strcat(glog , "labelled CRAWLEM MUCHE"); break;
        case 4: strcat(glog , "labelled ASTAGAFIRULLAH"); break;
        case 5: strcat(glog , "labelled OBOL"); break;
        case 6: strcat(glog , "labelled "); break;
        case 7: strcat(glog , "labelled "); break;
        }
        */

        strcat(glog, "labelled ");
        strcat(glog, make_name(item_da, it_plus, item_clas, 2));
        // have item_plus of scrolls = level on which it was created, and put item_type into make_name

        break; // end of scrolls


        case 7: // jewellery
//      icolour  = 14;


//      if (ident_lev > 0)

   if (ident_lev > 0)
   {
      if (it_plus >= 80)
      {
         strcat(glog, "cursed ");
      }
   }

   if (id [2] [item_typ] > 0)
        {

        if (ident_lev > 1 && item_typ == 1 | item_typ == 5 | item_typ == 11 | item_typ == 14 | item_typ == 15)
        {
                char gokh = it_plus - 50;
                //it_plus -= 50;
                if (gokh >= 0 && (gokh <= 80 | gokh >= 100)) strcat(glog, "+");
                itoa(gokh, tmp_quant, 10);
                if (gokh > 80) itoa((gokh - 100), tmp_quant, 10);
                strcat(glog, tmp_quant);
                strcat(glog, " ");
        }

        switch(item_typ)
        {
        case 0: strcat(glog , "ring of regeneration"); break;
        case 1: strcat(glog , "ring of protection"); break;
        case 2: strcat(glog , "ring of protection from fire"); break;
        case 3: strcat(glog , "ring of poison resistance"); break;
        case 4: strcat(glog , "ring of protection from cold"); break;
        case 5: strcat(glog , "ring of strength"); break;
        case 6: strcat(glog , "ring of slaying"); break;
        case 7: strcat(glog , "ring of see invisible"); break;
        case 8: strcat(glog , "ring of invisibility"); break;
        case 9: strcat(glog , "ring of hunger"); break;
        case 10: strcat(glog , "ring of teleportation"); break;
        case 11: strcat(glog , "ring of evasion"); break;
 case 12: strcat(glog , "ring of sustain abilities"); break;
 case 13: strcat(glog , "ring of sustenance"); break;
 case 14: strcat(glog , "ring of dexterity"); break;
 case 15: strcat(glog , "ring of intelligence"); break;
 case 16: strcat(glog , "ring of wizardry"); break;
 case 17: strcat(glog , "ring of magical power"); break;
 case 18: strcat(glog , "ring of levitation"); break;
 case 19: strcat(glog , "ring of life protection"); break;
 case 20: strcat(glog , "ring of protection from magic"); break;
 }

        break;
        } // end of if ident_lev

        if (item_da <= 13)
        {
//      randnum = random(14);
        switch(item_da )
        {
                case 0: strcpy(glog, "wooden ring"); break;
                case 1: strcpy(glog, "silver ring"); break;
                case 2: strcpy(glog, "golden ring"); break;
                case 3: strcpy(glog, "iron ring"); break;
                case 4: strcpy(glog, "steel ring"); break;
                case 5: strcpy(glog, "bronze ring"); break;
                case 6: strcpy(glog, "brass ring"); break;
                case 7: strcpy(glog, "copper ring"); break;
                case 8: strcpy(glog, "granite ring"); break;
                case 9: strcpy(glog, "ivory ring"); break;
                case 10: strcpy(glog, "bone ring"); break;
                case 11: strcpy(glog, "marble ring"); break;
                case 12: strcpy(glog, "jade ring"); break;
                case 13: strcpy(glog, "glass ring"); break;
//              case 14: strcpy(glog, "potion");
//              icolour [lopp] = 7;  break;
//              case 15: strcpy(glog, "potion");
//              icolour [lopp] = 7; break;

        } // end of switch(randnum)

        } else //ie if it's uncommon:
        {
//      secchar = random(13);
        switch(item_da / 13) // secondary characteristic of uncommon ring
        {
                case 0: strcpy(glog, "shiny "); break;
                case 1: strcpy(glog, "encrusted "); break;
                case 2: strcpy(glog, "glowing "); break;
                case 3: strcpy(glog, "tubular "); break;
                case 4: strcpy(glog, "runed "); break;
                case 5: strcpy(glog, "blackened "); break;
                case 6: strcpy(glog, "scratched "); break;
                case 7: strcpy(glog, "small "); break;
                case 8: strcpy(glog, "large "); break;
                case 9: strcpy(glog, "twisted "); break;

                case 10: strcpy(glog, "hairy "); break;
                case 11: strcpy(glog, "notched "); break;
                case 12: strcpy(glog, "knobbly "); break;
//*             case 13: strcpy(glog, " ");
//              icolour [lopp] = 14; break;
//              case 14: strcpy(glog, " ");
//              icolour [lopp] = 7;  break;
//              case 15: strcpy(glog, " ");
//              icolour [lopp] = 7;  break;

        } // end of switch(randnum)


//      randnum = random(14);
        switch(item_da % 13)
        {
        case 0: strcat(glog, "wooden ring"); break;
        case 1: strcat(glog, "silver ring"); break;
        case 2: strcat(glog, "golden ring"); break;
        case 3: strcat(glog, "iron ring"); break;
        case 4: strcat(glog, "steel ring"); break;
        case 5: strcat(glog, "bronze ring"); break;
        case 6: strcat(glog, "brass ring"); break;
        case 7: strcat(glog, "copper ring"); break;
        case 8: strcat(glog, "granite ring"); break;
        case 9: strcat(glog, "ivory ring"); break;
        case 10: strcat(glog, "bone ring"); break;
        case 11: strcat(glog, "marble ring"); break;
        case 12: strcat(glog, "jade ring"); break;
        case 13: strcat(glog, "glass ring"); break;
        } // end of switch(randnum)
        } // end of else.
        break;



case 10:
strcpy(glog, "");
if (ident_lev == 0)
{
switch(item_da / 10)
        {
        case 0: strcat(glog , ""); break;
        case 1: strcat(glog , "chunky "); break;
        case 2: strcat(glog , "thick "); break;
        case 3: strcat(glog , "thin "); break;
        case 4: strcat(glog , "wide "); break;
        case 5: strcat(glog , "glowing "); break;
        case 6: strcat(glog , "dog-eared "); break;
        case 7: strcat(glog , "oblong "); break;
        case 8: strcat(glog , "runed "); break;
        case 9: strcat(glog , " "); break;
        case 10: strcat(glog , " "); break;
        case 11: strcat(glog , " "); break;

 }
        switch(item_da % 10)
        {
//      case 0: strcat(glog , "book"); break;
        case 0: strcat(glog , "paperback book"); break;
        case 1: strcat(glog , "hardcover book"); break;
        case 2: strcat(glog , "leatherbound book"); break;
        case 3: strcat(glog , "metal-bound book"); break;
        case 4: strcat(glog , "papyrus book"); break;
        case 6: strcat(glog , " book"); break;
        case 7: strcat(glog , " book"); break;
 }
} else
        switch(item_typ)
        {
        case 0: strcat(glog , "book of minor magic"); break;
         case 1: strcat(glog , "book of minor magic"); break;
        case 2: strcat(glog , "book of minor magic"); break;
        case 3: strcat(glog , "book of minor magic"); break;
        case 4: strcat(glog , "book of minor magic"); break;
        case 5: strcat(glog , "book of minor magic"); break;
        case 6: strcat(glog , "book of minor magic"); break;
  case 7: strcat(glog , "book of summonings"); break;
 case 8: strcat(glog , "book of fire"); break;
 case 9: strcat(glog , "book of ice"); break;
 case 10: strcat(glog , "book of divinations"); break;
 case 11: strcat(glog , "book of spatial translocations"); break;
 case 12: strcat(glog , "book of enchantments"); break;
 case 13: strcat(glog , "book of poisonings"); break;
 case 14: strcat(glog , "book of greater conjurations"); break;
 case 15: strcat(glog , "book of necromancy"); break;
 case 16: strcat(glog , "book of enchantments"); break;
 case 17: strcat(glog , "book of holy invocations"); break;
 case 18: strcat(glog , "book of bodily restorations"); break;
 case 19: strcat(glog , "book of useful magic"); break;
 case 20: strcat(glog , "book of prayers"); break;
 case 21: strcat(glog , "book of clouds"); break;

        } // end switch
break;


case 11: // more magical devices
if (ident_lev == 0)
{
 switch(item_da) // gnarled
 {
  case 0: strcat(glog, "curved "); break;
  case 1: strcat(glog, "glowing "); break;
  case 2: strcat(glog, "thick "); break;
  case 3: strcat(glog, "thin "); break;
  case 4: strcat(glog, "long "); break;
  case 5: strcat(glog, "twisted "); break;
  case 6: strcat(glog, "jewelled "); break;
  case 7: strcat(glog, "runed "); break;
  case 8: strcat(glog, "smoking "); break;
  case 9: strcat(glog, " "); break;
  case 10: strcat(glog, " "); break;
  case 11: strcat(glog, " "); break;
  case 12: strcat(glog, " "); break;
  case 13: strcat(glog, " "); break;
  case 14: strcat(glog, " "); break;
  case 15: strcat(glog, " "); break;
  case 16: strcat(glog, " "); break;
  case 17: strcat(glog, " "); break;
  case 18: strcat(glog, " "); break;
  case 19: strcat(glog, " ");break;
  case 20: strcat(glog, " ");break;
  case 21: strcat(glog, " "); break;
  case 22: strcat(glog, " "); break;
  case 23: strcat(glog, " "); break;
  case 24: strcat(glog, " "); break;
  case 25: strcat(glog, " "); break;
  case 26: strcat(glog, " "); break;
  case 27: strcat(glog, " "); break;
  case 28: strcat(glog, " "); break;
  case 29: strcat(glog, " "); break;
 } // end switch
} // end if

//if (ident_lev > 0 && item_typ >= 10) strcat(glog, "spe

strcat(glog, "staff");

if (ident_lev > 0)
{
        switch(item_typ)
        {
        case 0: strcat(glog , " of wizardry"); break;
         case 1: strcat(glog , " of magical power"); break;
        case 2: strcat(glog , " of fire"); break;
        case 3: strcat(glog , " of cold"); break;
  case 4: strcat(glog , " of provide energy"); break; // crappy name. oh well

 case 10: strcat(glog , " of smiting"); break;
 case 11: strcat(glog , " of summoning"); break;
 case 12: strcat(glog , " of destruction"); break;
 case 13: strcat(glog , " of destruction"); break;
 case 14: strcat(glog , " of destruction"); break;
 case 15: strcat(glog , " of destruction"); break;
 case 16: strcat(glog , " of warding"); break;
 case 17: strcat(glog , " of discovery"); break;

        } // end switch
} // end if
break;


case 12:
strcpy(glog , "Orb of Zot"); break;
// have others, eg Orb of Zug etc
break;





case 13: // Gems
if (ident_lev == 0)
{
 switch(item_da)
 {
  case 0: strcat(glog, ""); break;
  case 1: strcat(glog, ""); break;
  case 2: strcat(glog, ""); break;
  case 3: strcat(glog, ""); break;
  case 4: strcat(glog, ""); break;
  case 5: strcat(glog, ""); break;
  case 6: strcat(glog, ""); break;
  case 7: strcat(glog, ""); break;
  case 8: strcat(glog, ""); break;
  case 9: strcat(glog, " "); break;
  case 10: strcat(glog, " "); break;
  case 11: strcat(glog, " "); break;
  case 12: strcat(glog, " "); break;
  case 13: strcat(glog, " "); break;
  case 14: strcat(glog, " "); break;
  case 15: strcat(glog, " "); break;
  case 16: strcat(glog, " "); break;
  case 17: strcat(glog, " "); break;
  case 18: strcat(glog, " "); break;
  case 19: strcat(glog, " ");break;
  case 20: strcat(glog, " ");break;
  case 21: strcat(glog, " "); break;
  case 22: strcat(glog, " "); break;
  case 23: strcat(glog, " "); break;
  case 24: strcat(glog, " "); break;
  case 25: strcat(glog, " "); break;
  case 26: strcat(glog, " "); break;
  case 27: strcat(glog, " "); break;
  case 28: strcat(glog, " "); break;
  case 29: strcat(glog, " "); break;
 } // end switch
} // end if

strcat(glog, " stone");

if (ident_lev > 0)
{
        switch(item_typ)
        {
  case 0: strcat(glog, ""); break;
  case 1: strcat(glog, ""); break;
  case 2: strcat(glog, ""); break;
  case 3: strcat(glog, ""); break;
  case 4: strcat(glog, ""); break;
  case 5: strcat(glog, ""); break;
  case 6: strcat(glog, ""); break;
  case 7: strcat(glog, ""); break;
  case 8: strcat(glog, ""); break;
  case 9: strcat(glog, ""); break;
  case 10: strcat(glog, ""); break;
  case 11: strcat(glog, ""); break;
  case 12: strcat(glog, ""); break;
  case 13: strcat(glog, ""); break;
  case 14: strcat(glog, ""); break;
  case 15: strcat(glog, ""); break;
  case 16: strcat(glog, ""); break;
  case 17: strcat(glog, ""); break;
  case 18: strcat(glog, ""); break;
  case 19: strcat(glog, "");break;
  case 20: strcat(glog, "");break;
  case 21: strcat(glog, ""); break;
  case 22: strcat(glog, ""); break;
  case 23: strcat(glog, ""); break;
  case 24: strcat(glog, ""); break;
  case 25: strcat(glog, ""); break;
  case 26: strcat(glog, ""); break;
  case 27: strcat(glog, ""); break;
  case 28: strcat(glog, ""); break;
  case 29: strcat(glog, ""); break;
        } // end switch
} // end if
break;


















        /*
        switch(item_da )
        {
        case 0: strcat(glog , "ring of regeneration"); break;
        case 1: strcat(glog , "ring of protection"); break;
        case 2: strcat(glog , ""); break; // make regen rarer.
        case 3: strcat(glog , " "); break;
        case 4: strcat(glog , " "); break;
        case 5: strcat(glog , ""); break;
        case 6: strcat(glog , ""); break;
        case 7: strcat(glog , ""); break;
        } break;
        */


        default: strcat(glog, "illegal item (");
        char ugug [4];
        itoa(item_clas, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",");
        itoa(item_typ, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ":");
        itoa(it_quant, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ")");
        break;


        } // end of switch?
//} // end of for lopp

if (strlen(glog) < 3)
{
        strcat(glog, "questionable item (c");
        char ugug [4];
        itoa(item_clas, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",t");
        itoa(item_typ, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",p");
        itoa(it_plus, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",d");
        itoa(item_da, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ":q");
        itoa(it_quant, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ")");


        //break;
}




if (it_quant > 1 && item_clas != 6 && item_clas != 8)
{
        strcat(glog, "s");
}



//free(glog);

//return glog;


} // end of char item_name_2









void housekeep(int mons_class [MNST], int mons_hp [MNST], unsigned char mons_hp_max [MNST], unsigned char mons_HD [MNST], int mons_AC [MNST], char mons_ev [MNST], float mons_speed [MNST], float mons_speed_inc [MNST], unsigned char mons_x [MNST], unsigned char mons_y [MNST], unsigned char mons_targ_1_x [MNST], unsigned char mons_targ_1_y [MNST], int mons_inv [MNST] [8], unsigned char mons_beh [MNST])
{
int i = 0;
int empty = -1;
int j = 0;

for (i = 0; i < MNST; i ++)
{
        if (empty != -1 && mons_class [i] != -1)
        {
        mons_class [empty] = mons_class [i];
        mons_hp [empty] = mons_hp [i];
        mons_hp_max [empty] = mons_hp_max [i];
        mons_AC [empty] = mons_AC [i];
        mons_ev [empty] = mons_ev [i];
        mons_HD [empty] = mons_HD [i];
        mons_speed [empty] = mons_speed [i];
        mons_speed_inc [empty] = mons_speed_inc [i];
        mons_beh [empty] = mons_beh [i];
        mons_x [empty] = mons_x [i];
        mons_y [empty] = mons_y [i];
        mons_hp [empty] = mons_hp [i];
        mons_targ_1_x [empty] = mons_targ_1_x [i];
        mons_targ_1_y [empty] = mons_targ_1_y [i];
        for (j = 0; j < 8; j ++)
        {
                mons_inv [empty] [j] = mons_inv [i] [j];
        }

        mons_class [i] = -1;
        i = empty;
        empty = -1;

        }



        if (mons_class [i] == -1)
        {
                empty = i;
        }

} // end of for i









} // end of void housekeep







int trap_item(int it_no, char icolour [ITEMS], char item_ident [ITEMS], unsigned char item_dam [ITEMS], unsigned char item_plus [ITEMS], char item_type [ITEMS], char item_class [ITEMS], int item_link [ITEMS], unsigned int item_quant [ITEMS], int igrid [80] [70], char iclass, char itype, char beam_x, char beam_y)
{
/*
if (iclass == -56)
{
cout << (int) iclass << "," << (int) itype << ":" << " @ " << (int) beam_x << "," << (int) beam_y;
//getch();
//getch();
//getch();
getch();
}
*/


if (igrid [beam_x] [beam_y] != 501)
        {
                if ((iclass == 1 | iclass == 4 | iclass == 6 | iclass == 8 | iclass == 9) && iclass == item_class [igrid [beam_x] [beam_y]] && itype == item_type [igrid [beam_x] [beam_y]] && item_plus [igrid [beam_x] [beam_y]] == 50 && item_dam [igrid [beam_x] [beam_y]] == 0)
                {
                        item_quant [igrid [beam_x] [beam_y]] ++;
                        return 0;
                }
        } // end of if igrid != 501

if (igrid [beam_x] [beam_y] != 501)
if (item_link [igrid [beam_x] [beam_y]] != 501)
if (item_link [item_link [igrid [beam_x] [beam_y]]] != 501)
 return 0;
// don't want to go overboard here.


        int o;

        for (o = 0; o < ITEMS; o++)
        {
  if (o >= 200) return 0;
                if (item_quant [o] == 0)
                {
                        //strcpy(fake_name [o], fake_name [hand_used]);
                        item_ident [o] = 0;
                        item_class [o] = iclass;
                        item_type [o] = itype;
                        item_plus [o] = 50;
                        item_dam [o] = 0;
                        icolour [o] = LIGHTCYAN;
                        //item_x [o] = beam_x; item_y [o] = beam_y; //x place position
                        item_quant [o] = 1;
                        item_link [o] = 501;
                        //it_no ++;
                        break;
                }
        } // end of o loop






        if (igrid [beam_x] [beam_y] == 501)
        {
                igrid [beam_x] [beam_y] = o;
        } else
        {
                int hug = igrid [beam_x] [beam_y];
                igrid [beam_x] [beam_y] = o;
                item_link [o] = hug;
        }
/*if (item_class [o] == -56)
{
cout << (int) item_class [o] << "," << (int) item_type [o] << ":" << (int) item_quant [o] << " @ " << (int) beam_x << "," << (int) beam_y;
getch();
getch();
getch();
getch();
}*/

//getch();

return 1;

}






/*
void put_show(int count_x, int count_y, int x_pos, int y_pos, unsigned char show_col [15] [15], unsigned char show [15] [15], int your_colour, int your_sign)
{
if (count_x != x_pos | count_y != y_pos)
{
        textcolor(show_col [count_x - x_pos + 7] [count_y - y_pos + 7]);
        //if (show [count_x - x_pos + 6] [count_y - y_pos + 6] >= 30 && show [count_x - x_pos + 6] [count_y - y_pos + 6] < 97) show [count_x - x_pos + 6] [count_y - y_pos + 6] -= 30;
        switch (show [count_x - x_pos + 7] [count_y - y_pos + 7])
                {
                case 0: putch(32); break;
                case 1: textcolor(BROWN); putch(177); break; // rock wall
                case 2: textcolor(BROWN); putch(177); break; // stone wall
                //                ^^^^^
                // later have level variables for each of them, as LIGHTGRAY walls may look slightly silly in hell

                case 3: putch(43); break;
                case 17: textcolor(LIGHTGRAY); putch(249); break;
                case 18: putch(60); break; // <
                case 19: putch(62); break; // >
                case 20: putch(39); break; // open door
                case 21: putch(60); break;
                case 22: putch(62); break;
                case 23: putch(60); break;
                case 24: putch(62); break;
                case 25: textcolor(11); putch(94); break; // ^ dart trap
                case 26: textcolor(LIGHTGRAY); putch(94); break;
                case 27: textcolor(MAGENTA); putch(94); break;
                case 28: putch(249); break; // undiscovered trap
                case 38: putch(41); break; // weapon )
                case 39: putch(91); break; // armour [
                case 40: putch(47); break; // food %
                case 41: putch(37); break; // wands / &c
                case 42: putch(43); break; // books +
                case 43: putch(63); break; // scroll ?
                case 44: putch(61); break; // ring = etc
                case 45: putch(33); break; // potions !
                case 46: putch(40); break; // stones
                case 47: putch(37); break; // tool (
                case 48: putch(37); break; // corpses part 1
                case 52: putch(36); break; // $ gold
                case 97: putch(97); break; // need to find a new starting number for the monsters, to cope with the +30 grid thing with more than 52 monsters.
                case 98: putch(98); break;
                case 99: putch(99); break;
                case 100: putch(100); break;
                case 101: putch(101); break;
                case 102: putch(102); break;
                case 103: putch(103); break; // g
                case 104: putch(104); break;
                case 105: putch(105); break;
                case 106: putch(106); break; // j
                case 107: putch(107); break;
                case 108: putch(108); break; // l
                case 109: putch(109); break; // m
                case 110: putch(110); break; // n
                case 111: putch(111); break; // o
                case 112: putch(112); break;
                case 113: putch(113); break;
                case 114: putch(114); break;
                case 115: putch(115); break;
                case 116: putch(116); break;
                case 117: putch(117); break;
                case 118: putch(118); break;
                case 119: putch(119); break;
                case 120: putch(120); break;
                case 121: putch(121); break;
                case 122: putch(122); break;
                case 123: putch(65); break; // A
                case 124: putch(66); break;
                case 125: putch(67); break;
                case 126: putch(68); break;
                case 127: putch(69); break;
                case 128: putch(70); break;
                case 129: putch(71); break; // G
                case 130: putch(72); break;
                case 131: putch(73); break;
                case 132: putch(74); break;
                case 133: putch(75); break; // K
                case 134: putch(76); break;
                case 135: putch(77); break;
                case 136: putch(78); break;
                case 137: putch(79); break;
                case 138: putch(80); break;
                case 139: putch(81); break;
                case 140: putch(82); break;
                case 141: putch(83); break;
                case 142: putch(84); break;
                case 143: putch(85); break;
                case 144: putch(86); break;
                case 145: putch(87); break;
                case 146: putch(88); break;
                case 147: putch(89); break;
                case 148: putch(90); break;
                case 149: putch(111); break; // orc warrior
                case 150: putch(75); break; // Kobold demonologist
                case 151: putch(111); break; // orc wizard
                case 152: putch(103); break; // goblin wizard
                case 153: putch(35); break;
                case 156: putch('X'); break;
                default: putch (show [count_x - x_pos + 7] [count_y - y_pos + 7]); break;
                }
//                              putch(show [count_x - x_pos + 6] [count_y - y_pos + 6]);
} else
      {
        textcolor (your_colour); // your colour!
        putch(your_sign); // You!
      }
}  // end of put_show(...)

*/










/*


void zapping(char ztype, char beam_colour, char beam_name [30], int beam_range, int beam_damage, char beam_flavour, int beam_type, int thing_thrown, int beam_hit)
{

// returns 1 if missile, 2 if beam

switch(ztype)
{
//if (inv_type [zap_device_2] == 0)
//{
        case 0: // flame
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        beam_colour = 4;
        strcpy(beam_name, "puff of flame");
        beam_range = random(5) + 7;
        beam_damage = 108; // should this be the same as magic missile?
        beam_hit = 60;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        beam_flavour = 2; // fire
        return;//1;
        //missile();
        //break;


        case 1: // frost
//      beam_source_y = y_pos;
        beam_colour = 15;
        strcpy(beam_name, "puff of frost");
        beam_range = random(5) + 7;
        beam_damage = 108; // should this be the same as magic missile?
        beam_hit = 60;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        //missile();
        beam_flavour = 3; // cold
        return;//1;
        //missile();
        //break;

        case 2: // slow monster
        strcpy(beam_name, "0");
        beam_range = random(5) + 6;
        beam_type = 0;
        beam_colour = 0;
        thing_thrown = 1;
        //beam();
        beam_flavour = 4; // magic
        return;//2;
        //beam();
        //break;

        case 3: // hasting
        strcpy(beam_name, "0");
        beam_range = random(5) + 6;
        beam_type = 0;
        beam_colour = 1;
        thing_thrown = 1;
        //beam();
        beam_flavour = 4; // magic
        return;//2;
        //beam();
        //break;

        case 4: // magic missile
//      strcpy(info [info_lines], "You hurl a ball of sizzling energy!");
//      incrl();
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        beam_colour = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
        strcpy(beam_name, "magic missile");// inv_name [throw_2]);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1)
//      {
//              beam_damage = property [inv_class [throw_2]] [inv_type [throw_2]] [0];
//              beam_damage += inv_plus [throw_2];
//      }
//       else beam_damage = 2; // later put mass thing here.
        beam_range = random(5) + 7;
        beam_damage = 107;
        beam_hit = 80;
        beam_type = 35;
//      beam_hit = strength + (0.5 * dex) + (f_abil);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1) beam_hit += inv_plus [throw_2];
        //beam_hit *= random(200) / 100;
        thing_thrown = 3;
        beam_flavour = 1; // a magical missile
        return;//1;
        //missile();
//      break;

        case 5: // healing
        strcpy(beam_name, "0");
        beam_range = random(5) + 6;
        beam_type = 0;
        beam_colour = 2;
        thing_thrown = 1;
        beam_flavour = 4; // magic
        return;//2;
        //beam();
        //break;

        case 6: // paralysis
        strcpy(beam_name, "0");
        beam_range = random(5) + 6;
        beam_type = 0;
        beam_colour = 3;
        thing_thrown = 1;
        beam_flavour = 4; // magic
        return;//2;
        //beam();
        //break;

        case 7:
        strcpy(beam_name, "bolt of fire");
        beam_range = random(10) + 5;
        beam_damage = 112;
        beam_colour = 4;
        beam_type = 35; // 35
        thing_thrown = 1;
        beam_flavour = 2; // fire
        return;//2;
        //beam();
        //break;
//}


//if (inv_type [zap_device_2] == 1)
//{
        case 8:
        strcpy(beam_name, "bolt of cold");
        beam_range = random(10) + 5;
        beam_damage = 112;
        beam_colour = 15;
        beam_type = 35;
        thing_thrown = 1;
        beam_flavour = 3; // cold
        return;//2;
        //beam();
        //break;

        case 9: // confusion
        strcpy(beam_name, "0");
        beam_range = random(5) + 6;
        beam_type = 0;
        beam_colour = 4;
        thing_thrown = 1;
        beam_flavour = 4; // magic
        return;//2;
        //beam();
        //break;

        case 10: // make invisible
        strcpy(beam_name, "0");
        beam_range = random(5) + 6;
        beam_type = 0;
        beam_colour = 5;
        thing_thrown = 1;
        beam_flavour = 4; // magic
        return;//2;
        //beam();
        //break;

        case 11: // digging
        strcpy(beam_name, "0");
        beam_range = random(5) + 6;
        beam_type = 0;
        beam_colour = 6;
        thing_thrown = 1;
        beam_flavour = 4;
        return;//2;
        //beam();
        //break;


        case 12: // fireball
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        beam_colour = 4;
        strcpy(beam_name, "fireball");
        beam_range = random(5) + 7;
        beam_damage = 15; // should this be the same as magic missile?
        beam_hit = 40;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        beam_flavour = 10; // fire
        cout << "Fireball!"; getch();
        return;//1;
        //missile();
        //break;

        case 13: // teleportation
        strcpy(beam_name, "0");
        beam_range = random(5) + 6;
        beam_type = 0;
        beam_colour = 7; // 6 is used by digging
        thing_thrown = 1;
        beam_flavour = 4; // magic
        return;//2;
        //beam();
        //break;


} // end of switch

cout << "Haven't found anything.";
getch();

return;//0;

}

*/






//int eaten(char info [78] [78], int inflis, int *hung, int delay_t, int delay_doing, int food_eat_3)
int eating(char info [78] [78], int inflis, int func_pass [10], int food_eat_3)
{
// make it int eaten(...), just in case...

switch(food_eat_3)
{
        case 0:
        strcpy(info [inflis], "That meat ration really hit the spot!");
        //incrl();
        //hung += 2500;
        //delay_t = 4;
        //delay_doing = 0;
        func_pass [0] += 5000;
        func_pass [1] = 4;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case 1:
        strcpy(info [inflis], "That bread ration really hit the spot!");
        //incrl();
        //hung += 2200;
        //delay_t = 4;
        //delay_doing = 0;
        func_pass [0] += 4400;
        func_pass [1] = 4;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case 3: // apple
        strcpy(info [inflis], "Mmmm... Yummy apple.");
        //incrl();
        //hung += 300;
        //delay_t = 1;
        //delay_doing = 0;
        func_pass [0] += 700;
        func_pass [1] = 1;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case 2: // pear
        strcpy(info [inflis], "Mmmm... Yummy pear.");
        //incrl();
        //hung += 300;
        //delay_t = 1;
        //delay_doing = 0;
        func_pass [0] += 700;
        func_pass [1] = 1;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case 4:
        strcpy(info [inflis], "That choko was very bland.");
        //incrl();
        //hung += 300;
        //delay_t = 1;
        //delay_doing = 0;
        func_pass [0] += 600;
        func_pass [1] = 1;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case 5:
        strcpy(info [inflis], "That honeycomb was delicious.");
        func_pass [0] += 2000;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 6:
        strcpy(info [inflis], "That royal jelly was delicious!");
        func_pass [0] += 5000;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 7: // maybe a nasty side-effect from RD's book?
        strcpy(info [inflis], "That snozzcumber tasted truly putrid!");
        func_pass [0] += 1500;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 8:
        strcpy(info [inflis], "Mmm... ");
 switch(random2(9))
        {
  case 0: strcat(info [inflis], "Ham and pineapple."); break;
  case 1: strcat(info [inflis], "Extra thick crust."); break;
  case 2: strcat(info [inflis], "Vegetarian."); break;
  case 3: strcat(info [inflis], "Pepperoni."); break;
  case 4:  strcpy(info [inflis], "Yeuchh - Anchovies!"); break;
  case 5: strcat(info [inflis], "Cheesy."); break;
  case 6: strcat(info [inflis], "Supreme."); break;
  case 7: strcat(info [inflis], "Super Supreme!"); break;
  case 8: strcat(info [inflis], "Chicken."); break;
        }
        func_pass [0] += 1500;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 9:
        strcpy(info [inflis], "That apricot was delicious!");
        func_pass [0] += 700;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 10:
        strcpy(info [inflis], "That orange was delicious!");
 if (random2(8) == 0) strcat(info [inflis], " Even the peel tasted good!");
        func_pass [0] += 1000;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 11:
        strcpy(info [inflis], "That banana was delicious!");
 if (random2(8) == 0) strcat(info [inflis], " Even the peel tasted good!");
        func_pass [0] += 1000;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 12:
        strcpy(info [inflis], "That strawberry was delicious!");
        func_pass [0] += 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 13:
        strcpy(info [inflis], "That rambutan was delicious!");
        func_pass [0] += 600;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 14:
        strcpy(info [inflis], "That lemon was rather sour... But delicious nonetheless!");
        func_pass [0] += 1000;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 15:
        strcpy(info [inflis], "That grape was delicious!");
        func_pass [0] += 100;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 16:
        strcpy(info [inflis], "That sultana was delicious! (but very small)");
        func_pass [0] += 70; // won't rescue you from starvation
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 17:
        strcpy(info [inflis], "That lychee was delicious!");
        func_pass [0] += 600;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 18:
        strcpy(info [inflis], "That beef jerky was delicious!");
 if (random2(4) == 0) strcpy(info [inflis], "That beef jerky was jerk-a-riffic!");
        func_pass [0] += 1500;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 19:
        strcpy(info [inflis], "Mmm... ");
 switch(random2(9))
        {
  case 0: strcat(info [inflis], "Cheddar."); break;
  case 1: strcat(info [inflis], "Edam."); break;
  case 2: strcat(info [inflis], "Wensleydale."); break;
  case 3: strcat(info [inflis], "Camembert."); break;
  case 4: strcpy(info [inflis], "Goat's milk cheese."); break;
  case 5: strcat(info [inflis], "Fruit cheese."); break;
  case 6: strcat(info [inflis], "Mozzarella."); break;
  case 7: strcat(info [inflis], "Sheep cheese."); break;
  case 8: strcat(info [inflis], "Yak cheese."); break;
        }
        func_pass [0] += 1200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 20:
        strcpy(info [inflis], "That sausage was delicious!");
        func_pass [0] += 1200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

// That code was inefficient!

} // end of switch


}// end of int eaten(...)





char *weird_colours(unsigned char coll)
{

char wc [30];

strcpy(wc, "");

//if (coll > 15)
//{
switch(coll / 16)
{
        case 0: strcat(wc, "brilliant "); break;
        case 1: strcat(wc, "pale "); break;
        case 2: strcat(wc, "mottled "); break;
        case 3: strcat(wc, "shimmering "); break;
        case 4: strcat(wc, "bright "); break;
        case 5: strcat(wc, "dark "); break;
        case 6: strcat(wc, "shining "); break;
        case 7: strcat(wc, "brilliant "); break;
        case 8: strcat(wc, "pale "); break;
        case 9: strcat(wc, "mottled "); break;
        case 10: strcat(wc, "shimmering "); break;
        case 11: strcat(wc, "bright "); break;
        case 12: strcat(wc, "dark "); break;
        case 13: strcat(wc, "shining "); break;

        // Must start with a consonant!


//      case 7: strcat(wc, " "); break;
//      case 8: strcat(wc, " "); break;

        default: strcat(wc, "faint "); break;
}
//} // end of if coll > 15

while(coll > 17)
{
        coll -= 10;
}

switch(coll)
{
        case 0: strcat(wc, "red"); break;
        case 1: strcat(wc, "purple"); break;
        case 2: strcat(wc, "green"); break;
        case 3: strcat(wc, "orange"); break;
        case 4: strcat(wc, "magenta"); break;
        case 5: strcat(wc, "black"); break;
        case 6: strcat(wc, "grey"); break;
        case 7: strcat(wc, "silver"); break;
        case 8: strcat(wc, "gold"); break;
        case 9: strcat(wc, "pink"); break;
        case 10: strcat(wc, "yellow"); break;
        case 11: strcat(wc, "white"); break;
        case 12: strcat(wc, "brown"); break;
        case 13: strcat(wc, "aubergine"); break;
        case 14: strcat(wc, "ochre"); break;
        case 15: strcat(wc, "leaf green"); break;
        case 16: strcat(wc, "mauve"); break;
        case 17: strcat(wc, "azure"); break;
//      case 18: strcat(wc, ""); break;
//      case 19: strcat(wc, ""); break;
}

/*char tm [2];

itoa(coll, tm, 10);
strcat(wc, tm);*/

return wc;

}












/*
int func_pass [10];
char str_pass [50];
*/


int mons_spells(char spell_cast, int func_pass [10], char beam_name [30])
{

/*
beam_colour = func_pass [0];
beam_range = func_pass [1];
beam_damage = func_pass [2];
beam_hit = func_pass [3];
beam_type = func_pass [4];
beam_flavour = func_pass [5];
thing_thrown = func_pass [6];
*/

switch(spell_cast)
{

case 0: // magic missile
//strcpy(info [info_lines], "The ");
//strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
//strcat(info [info_lines], " hurls a ball of sizzling energy!");
//incrl();
func_pass [0] = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
strcpy(beam_name, "magic dart");// inv_name [throw_2]);
func_pass [1] = random2(5) + 7;
func_pass [2] = 103;
func_pass [3] = 1500;
func_pass [4] = 35;
func_pass [6] = 4;
//return 0;
func_pass [5] = 1; // a magical missile
return 0;
//break;

case 1: // flame
//strcpy(info [info_lines], "The ");
//strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
//strcat(info [info_lines], " hurls a puff of flame!");
//incrl();
func_pass [0] = 4;
strcpy(beam_name, "puff of flame");
func_pass [1] = random2(5) + 7;
func_pass [2] = 104; // should this be the same as magic missile?
func_pass [3] = 60;
func_pass [4] = 42; // maybe make return 0 put a # on wherever it hits?
func_pass [6] = 4;
func_pass [5] = 2; // fire
return 0;
//break;


case 2: // frost
//strcpy(info [info_lines], "The ");
//strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
//strcat(info [info_lines], " hurls a puff of frost!");
//incrl();
func_pass [0] = 15;
strcpy(beam_name, "puff of frost");
func_pass [1] = random2(5) + 7;
func_pass [2] = 104; // should this be the same as magic missile?
func_pass [3] = 60;
func_pass [4] = 42; // maybe make return 0 put a # on wherever it hits?
func_pass [6] = 4;
func_pass [5] = 3; // cold
return 0;
//break;

case 3: // paralysis
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 3;
func_pass [6] = 4;
func_pass [5] = 4; // magic
return 1;
//break;

case 4: // slow monster
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 0;
func_pass [6] = 4;
func_pass [5] = 4; // magic
return 1;
//break;

case 5: // haste self
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 1;
func_pass [6] = 4;
func_pass [5] = 4; // magic
return 1;

case 6: // slow monster
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 4;
func_pass [6] = 4;
func_pass [5] = 4; // magic
return 1;

case 7: // venom bolt
strcpy(beam_name, "bolt of poison");
func_pass [1] = random2(10) + 8;
func_pass [2] = 108;
func_pass [0] = LIGHTGREEN;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 6; // poison
func_pass [7] = 1;
func_pass [3] = 5 + random2(func_pass [8]) / 80;
return 1;

case 8:
strcpy(beam_name, "bolt of fire");
func_pass [1] = random2(10) + 5;
func_pass [2] = 112;
func_pass [0] = 4;
func_pass [4] = 35; // 35
func_pass [6] = 2;
func_pass [5] = 2; // fire
func_pass [3] = 8; // hit
return 1;
//break;


case 9:
strcpy(beam_name, "bolt of cold");
func_pass [1] = random2(10) + 5;
func_pass [2] = 112;
func_pass [0] = 15;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 3; // cold
func_pass [3] = 8; // hit
return 1;

case 10: // lightning
strcpy(beam_name, "bolt of lightning");
func_pass [1] = random2(10) + 8;
func_pass [2] = 115;
func_pass [0] = LIGHTCYAN;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 5; // electricity
func_pass [7] = 1;
func_pass [3] = 50;
return 1;

case 11: // make invisible
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 5;
func_pass [6] = 2;
func_pass [5] = 4; // magic
return 1;

case 12: // fireball
func_pass [0] = 4;
strcpy(beam_name, "fireball");
func_pass [1] = random2(5) + 7;
func_pass [2] = 15;
func_pass [3] = 40;
func_pass [4] = 42;
func_pass [6] = 3;
func_pass [5] = 10; // fire
return 0;

case 13: // healing
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 2;
func_pass [6] = 2;
func_pass [5] = 4; // magic
return 1;

case 14: // teleportation
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 7; // 6 is used by digging
func_pass [6] = 2;
func_pass [5] = 4; // magic
return 1;

case 15: // teleport target away
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 7; // 6 is used by digging
func_pass [6] = 2;
func_pass [5] = 4; // magic
return 1;

case 16:
return 0; // blink

case 17: // splinters
strcpy(beam_name, "spray of metal splinters");
func_pass [1] = random2(10) + 8;
func_pass [2] = 117;
func_pass [0] = CYAN;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 17; // whatever
func_pass [7] = 1;
func_pass [3] = 55 + random2(func_pass [8]) / 80;
return 1;

case 18: // digging
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 4 + random2(func_pass [8]) / 10;
func_pass [4] = 0;
func_pass [0] = 6;
func_pass [6] = 1;
func_pass [5] = 4;
return 1;

case 19: // negative energy
strcpy(beam_name, "bolt of negative energy");
func_pass [1] = random2(10) + 8;
func_pass [2] = 107;
func_pass [0] = DARKGRAY;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 7; // -ve energy
func_pass [7] = 1;
func_pass [3] = 5 + random2(func_pass [8]) / 80;
return 1;
// 20, 21 are used

case 22: // mystic blast
func_pass [0] = 13;
strcpy(beam_name, "orb of energy");
func_pass [1] = random2(5) + 7;
func_pass [2] = 115;
func_pass [3] = 15;
func_pass [4] = 35;
func_pass [6] = 4;
func_pass [5] = 1; // a magical missile
return 0;

// 23 is brain feed

case 26: // ball of steam
func_pass [0] = LIGHTGRAY;
strcpy(beam_name, "ball of steam");
func_pass [1] = random2(5) + 7;
func_pass [2] = 105;
func_pass [3] = 11;
func_pass [4] = 35;
func_pass [6] = 4;
func_pass [5] = 2; // fire - I think this is appropriate
return 0;

// 27 is summon devils

case 49: // fiend's hellfire
strcpy(beam_name, "hellfire");
func_pass [1] = random2(10) + 5;
func_pass [2] = 20;
func_pass [0] = 20;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 10; // hellfire
return 1;




}



}
























/*

        case 0: // flame
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        func_pass [0] = 4;
        strcpy(str_pass, "puff of flame");
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 104 + func_pass [8] / 23; // should this be the same as magic missile? no
        func_pass [3] = 60;
        func_pass [4] = 42; // maybe make missile() put a # on wherever it hits?
        func_pass [6] = 3;
        func_pass [5] = 2; // fire
        func_pass [7] = 1; // wand_id
        return 1;
        //break;


        case 1: // frost
//      beam_source_y = y_pos;
        func_pass [0] = 15;
        strcpy(str_pass, "puff of frost");
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 104 + func_pass [8] / 23; // should this be the same as magic missile?
        func_pass [3] = 60;
        func_pass [4] = 42; // maybe make missile() put a # on wherever it hits?
        func_pass [6] = 3;
        //return 1;
        func_pass [5] = 3; // cold
        func_pass [7] = 1;
        return 1;
        //break;

        case 2: // slow monster
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 0;
        func_pass [6] = 1;
 func_pass [2] = 50;
        //beam();
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 3: // hasting
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 1;
        func_pass [6] = 1;
        //beam();
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 4: // magic missile
        //strcpy(info [info_lines], "You hurl a ball of sizzling energy!");
        //incrl();
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        func_pass [0] = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
        strcpy(str_pass, "magic missile");// inv_name [throw_2]);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1)
//      {
//              func_pass [2] = property [inv_class [throw_2]] [inv_type [throw_2]] [0];
//              func_pass [2] += inv_plus [throw_2];
//      }
//       else func_pass [2] = 2; // later put mass thing here.
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 103 + func_pass [8] / 100;
        func_pass [3] = 1500;
        func_pass [4] = 35;
//      func_pass [3] = strength + (0.5 * dex) + (f_abil);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1) func_pass [3] += inv_plus [throw_2];
        //func_pass [3] *= random2(200) / 100;
        func_pass [6] = 3;
        func_pass [5] = 1; // a magical missile
        func_pass [7] = 1;
        return 1;
        //break;

        case 5: // healing
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 2;
        func_pass [6] = 1;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 6: // paralysis
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 3;
        func_pass [6] = 1;
        func_pass [5] = 4; //  magic
 func_pass [2] = 0; //func_pass [9];
        return 2;
        //break;

        case 7:
        strcpy(str_pass, "bolt of fire");
        func_pass [1] = random2(10) + 5;
        func_pass [2] = 108 + func_pass [8] / 10;
        func_pass [0] = 4;
        func_pass [4] = 35; // 35
        func_pass [6] = 1;
        func_pass [5] = 2; // fire
        func_pass [7] = 1;
 func_pass [3] = 5 + random2(func_pass [8]) / 80;
        return 2;
        //break;
//}


//if (inv_type [zap_device_2] == 1)
//{
        case 8:
        strcpy(str_pass, "bolt of cold");
        func_pass [1] = random2(10) + 5;
        func_pass [2] = 112 + func_pass [8] / 10;
        func_pass [0] = WHITE; //LIGHTCYAN;
        func_pass [4] = 35;
        func_pass [6] = 1;
        func_pass [5] = 3; // cold
        func_pass [7] = 1;
 func_pass [3] = 5 + random2(func_pass [8]) / 80;
        return 2;
        //break;

        case 9: // confusion
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 4;
        func_pass [6] = 1;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 10: // make invisible
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 5;
        func_pass [6] = 1;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 11: // digging
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 4 + random2(func_pass [8]) / 10;
        func_pass [4] = 0;
        func_pass [0] = 6;
        func_pass [6] = 1;
        func_pass [5] = 4;
        return 2;
        //break;


        case 12: // fireball
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        func_pass [0] = 4;
        strcpy(str_pass, "fireball");
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 15; // should this be the same as magic missile?
        func_pass [3] = 40;
        func_pass [4] = 42;
        func_pass [6] = 3;
        func_pass [5] = 10; // fire
        return 1;
        //break;

        case 13: // teleportation
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 7; // 6 is used by digging
        func_pass [6] = 1;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 14: // lightning
        strcpy(str_pass, "bolt of lightning");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 115 + func_pass [8] / 9;
        func_pass [0] = LIGHTCYAN;
        func_pass [4] = 35;
        func_pass [6] = 1;
        func_pass [5] = 5; // electricity
        func_pass [7] = 1;
 func_pass [3] = 50; // beam hit - look at it!
        return 2;

   case 15: // polymorph
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 8;
        func_pass [6] = 1;
        func_pass [5] = 4; // magic
        return 2;

        case 16: // venom bolt
        strcpy(str_pass, "bolt of poison");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 105 + func_pass [8] / 9;
        func_pass [0] = LIGHTGREEN;
        func_pass [4] = 35;
        func_pass [6] = 1;
        func_pass [5] = 6; // poison
        func_pass [7] = 1;
 func_pass [3] = 5 + random2(func_pass [8]) / 80;
        return 2;





*/




















/*


void monster_grid(unsigned char no_mons, unsigned char mons_x [MNST], unsigned char mons_y [MNST], unsigned char mons_beh [MNST], int invis, char see_invis, int mons_class [MNST], char mcolour [MNST], int x_pos, int y_pos, unsigned char show [15] [15], unsigned char show_col [15] [15], char mons_ench [MNST] [3], int func_pass [10], unsigned char mons_sec [MNST])
{
int s; // a loop thing
int mnc = 0;

for (s = 0; s < MNST; s++)
{

if (mnc > no_mons) break;

if (mons_class [s] != -1)
{
        mnc++;

        if (mons_x [s] > x_pos - 7 && mons_x [s] < x_pos + 7 && mons_y [s] > y_pos - 7 && mons_y [s] < y_pos + 7 && show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] != 0)
        // Put the bit commented out on the previous line back to restore shadow checking for monsters
        {
        if ((mons_beh [s] == 0 | mons_beh [s] == 2) && invis == 0) mons_beh [s] = 1; // put stealth/invis here.

//      if (mons_ench [s] [2] == 6) && show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] > 70)
        if (mons_ench [s] [2] == 6 && see_invis == 0)
        {
                if (show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] > 50)
                        show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] -= 50;
                continue;
        } else func_pass [0] = 0;

        show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mons_class [s] + 97;
        show_col [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mcolour [mons_class [s]];

        if (mons_class [s] == 49) show_col [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mons_sec [s];


//      if (mons_class [s] >= 54) show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = 151;
// Demons: change 54 in above line to class of first demon


        //grid [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] += 30;
        //mons_targ_1_x [s] = x_pos; // monster is after you now!
        }
}
} // end of for i above


/*
void monster_grid(void)
{

for (i = 0; i < 120; i++)
{
if (mons_class [i] != -1)
{
        if (mons_x [i] > x_pos - 6 && mons_x [i] < x_pos + 6 && mons_y [i] > y_pos - 6 && mons_y [i] < y_pos + 6 && show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] != 0)
        // Put the bit commented out on the previous line back to restore shadow checking for monsters
        {
        show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] = mons_class [i] + 97;
        show_col [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] = mcolour [mons_class [i]];
        //grid [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] += 30;
        }
}
} // end of for i above




} // end of monster_grid()

*/


//} // end of monster_grid()





char get_ch(void)
{

char ch_got;

ch_got = getch();

if (ch_got == 0)
{
        ch_got = getch();
        return 0;
}

return ch_got;

}






/*
void mons_ench_f(char mons_ench [MNST] [3], char mons_speed [MNST], char mons_ench_1 [MNST], char mons_beh [MNST], int i)
{

char p;

for (p = 0; p < 3; p ++)
{
switch (mons_ench [i] [p])
{
        case 1: // slow
        if (random2(60) == 0)
        {
                if (mons_speed [i] >= 100)
                {
                        mons_speed [i] = ((mons_speed [i] - 100) * 2) + 100;
                } else mons_speed [i] *= 2;
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;

        case 2: // haste
        if (random2(60) == 0)
        {
                if (mons_speed [i] >= 100)
                {
                        mons_speed [i] = ((mons_speed [i] - 100) / 2) + 100;
                } else mons_speed [i] /= 2;
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;


        case 3: // paralysis
        //mons_targ_1_x [i] = mons_x [i];
        //mons_targ_1_y [i] = mons_y [i];
        //mons_beh [i] = 0;
        //mons_speed [i] += 100;
        if (random2(20) == 0)
        {
//      cprintf("iggy");
//      getch();
           mons_speed [i] -= 100;
                //mons_ev [i] += 5;
                mons_ench [i] [p] = 0;
                //mons_speed [i] /= 2;
                //mons_ench [i] [p] = 0;
                //mons_beh [i] =
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;


        case 4: // fear
        mons_beh [i] = 3;
        if (random2 (40) == 0)
        {
        mons_beh [i] = 1; // reset to monster's original behaviour
        mons_ench [i] [p] = 0;
        if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;


        case 5: // confusion
        mons_beh [i] = 4;
        if (random2 (40) == 0)
        {
                mons_beh [i] = 1;
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                mons_ench_1 [i] = 0;
        }
        break;


        case 6: // invisibility
        if (random2 (40) == 0)
        {
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                mons_ench_1 [i] = 0;
        }
        break;

} // end of switch
} // end of for p


}
*/



int exp_needed(int lev)
{

switch(lev)
{
        case 2: return 10;
        case 3: return 20;
        case 4: return 40;
        case 5: return 80;
        case 6: return 160;
        case 7: return 320;
        case 8: return 640;
        case 9: return 1280;
        case 10: return 2560;
        case 11: return 5120;
        case 12: return 10240;
//      case 13: return 20480;
//      case 14: return 40960;
//      case 15: return 81920;
//      case 16: return 163840;
//      case 17: return 327760;
 default: return 10240 * (lev - 11);
}


}

int random2(unsigned int rmax)
{
        //return random(rmax); // for BC

   if (rmax == 0) return 0;

        return random() % rmax; // for DJGPP
}



int magic_ability(int mag_abil, int intel)
{

int retv = mag_abil * intel;

retv /= 10;


return retv;

}


unsigned char check_item_knowledge(void)
{


   char st_pass [60];

   int i;
   char buffer[2400];
   int j;
   char lines = 0;
   unsigned char anything = 0;
   unsigned char strng [5] = "";
   char ft;

   char yps = 0;

   gettext(35, 1, 80, 25, buffer);

   strcpy(st_pass, "");

   window(35, 1, 80, 25);

   clrscr();



int inv_count = 0;
unsigned char ki;



for (i = 0; i < 4; i++)
{
 for (j = 0; j < 30; j ++)
 {
        if (id [i] [j] != 0)
        {
                inv_count++;
        }
 }
}

if (inv_count == 0)
{
   cprintf("You don't recognise anything yet!");
   if (getch() == 0) getch();
   goto putty;
}
textcolor(BLUE);
cprintf("  You recognise:");
textcolor(LIGHTGRAY);
lines++;


for (i = 0; i < 4; i ++)
        {
switch(i)
{
 case 0: ft = 3; break; // magic devices
 case 1: ft = 6; break; // scrolls
 case 2: ft = 7; break; // rings
 case 3: ft = 8; break; // potions
}

        for (j = 0; j < 30; j++)
        {

                if (lines > 23 && inv_count > 0)
                {
                        gotoxy(1,25);
                        cprintf("-more-");
                        ki = getch();
                        if (ki == 27)
                        {
                                puttext(35, 1, 80, 25, buffer);
                                return 27;
                        }
                        if (ki >= 65 && ki < 123)
                        {
                                puttext(35, 1, 80, 25, buffer);
                                return ki;
                        }
                        if (ki == 0) ki = getch();
                        lines = 0;
                        clrscr();
                        gotoxy(1,1);
                        anything = 0;
                }

                if (id [i] [j] == 1)
                {
                        anything ++;

                        if (lines > 0) cprintf("\n\r");
                        lines++;
                                cprintf(" ");

   yps = wherey();

//   item_name_2(i, j, 0, 0, 1, 3, 3, st_pass);
//char item_name_2(char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog [60])
item_name_2(ft, j, 0, 0, 1, 0, st_pass);

                        cprintf(st_pass);

         inv_count --;

   if (wherey() != yps) lines++;

                }
        } // end of j loop
}



   if (anything > 0)
   {
           ki = getch();
           //ki = getch();
           //ki = anything;
           if (ki >= 65 && ki < 123)
           {
                   puttext(35, 1, 80, 25, buffer);
                   return ki;
           }
           if (ki == 0) ki = getch();
           puttext(35, 1, 80, 25, buffer);
           return anything;
   }

   putty : puttext(35, 1, 80, 25, buffer);

   return ki;

} // end of check_item_knowledge










char spell_list(char spells [25])
{
char st_pass [60];

   int i;
   char buffer[2400];
   int j;
   char lines = 0;
   unsigned char anything = 0;
   unsigned char strng [5] = "";
   char ft;
   char ki;

   //clrscr();
//   for (i = 0; i <= 20; i++)
//      cprintf("Line #%d\r\n", i);
   gettext(35, 1, 80, 25, buffer);

   strcpy(st_pass, "");

   window(35, 1, 80, 25);

   clrscr();


//void inventory(int item_class_inv)

//{

/*int Inv_class2 [10];
int inv_count = 0;
unsigned char ki;

for (i = 0; i < 10; i++)
{
        Inv_class2 [i] = 0;
}


//if (inv_no != 0)
//{

//strcpy(info [info_lines], " You are carrying"); info_lines++;

for (i = 0; i < 52; i++)
{
        if (inv_quant [i] != 0)
        {
                Inv_class2 [inv_class [i]] ++; // adds up number of each class in invent.
                inv_count++;
        }
}


if (item_class_inv != -1)
{
for (i = 0; i < 10; i++)
{
        if (item_class_inv == 1 && i == 0) i++;
        if (item_class_inv != i) Inv_class2 [i] = 0;
}
}
*/

//cprintf("hello!");
//abort();
//if (item_class_inv > 1) Inv_class2 [0] = 0;


//if ((item_class_inv == -1 && inv_count > 0) | (item_class_inv != -1 && Inv_class2 [item_class_inv] > 0) | (item_class_inv == 1 && (Inv_class2 [0] > 0 | Inv_class2 [1] > 0)))
//{

//if (item_class_inv != 1) //this is so you can get the '?' invent from throw_it
//{
cprintf("  Your spells:                  Level");
lines++;
//info_lines++;
//}


//for (i = 0; i < 9; i ++)
//      {
//      if (Inv_class2 [i] != 0)
//      {
//      cprintf("\n\r");
        //lines++;
//      cprintf(" ");
        if (lines > 22)
        {
                //gotoxy(1,25);
                //cprintf("\n\r-more-");
                //ki = getch();
                //lines = 0;
                //clrscr();
                //gotoxy(1,1);
                gotoxy(1,25);
                cprintf("-more-");
                ki = getch();
                if (ki == 27)
                {
                        puttext(35, 1, 80, 25, buffer);
                        return 27;
                }
                if (ki >= 65 && ki < 123)
                {
                        puttext(35, 1, 80, 25, buffer);
                        return ki;
                }
                if (ki == 0) ki = getch();
                lines = 0;
                clrscr();
                gotoxy(1,1);
                anything = 0;

        }
        if (lines > 0) cprintf("\n\r ");
  /*    textcolor(BLUE);
        switch(i)
        {
        case 0: cprintf("Hand weapons"); break;
        case 1: cprintf("Missiles"); break;
        case 2: cprintf("Armour"); break;
        case 3: cprintf("Magical devices"); break;
        case 4: cprintf("Comestibles"); break;
        case 5: cprintf("Books"); break;
        case 6: cprintf("Scrolls"); break;
        case 7: cprintf("Jewellery"); break;
        case 8: cprintf("Potions & substances"); break;
        case 9: cprintf("Gems"); break;
        }
        textcolor(LIGHTGRAY);
        //cprintf("\n\r");
        lines++;*/

        for (j = 0; j < 25; j++)
        {

                if (lines > 23)
                {
                        gotoxy(1,25);
                        cprintf("-more-");
                        ki = getch();
                        if (ki == 27)
                        {
                                puttext(35, 1, 80, 25, buffer);
                                return 27;
                        }
                        if (ki >= 65 && ki < 123)
                        {
                                puttext(35, 1, 80, 25, buffer);
                                return ki;
                        }
                        if (ki == 0) ki = getch();
                        lines = 0;
                        clrscr();
                        gotoxy(1,1);
                        anything = 0;
                }

                if (spells [j] > -1)
                {

                        anything ++;
                        //strcpy(info [info_lines], " ");
                        //strncpy(info [info_lines], letters [j], 1);
                        //strcat(info [info_lines], " - ");
                        //printf(" ");
                        if (lines > 0) cprintf("\n\r");
                        lines++;
                                cprintf(" ");
                                if (j < 26) ft = (char) j + 97;
                                        else ft = (char) j  + 65;
                                //itoa(j, strng, 10);
                                strng [0] = ft;
                                //strng [0] += 49;
                                cprintf(strng);
                                //cout << " " << ft;
                        cprintf(" - ");


                        //strcat(info [info_lines], item_name(inv_class [j], inv_type [j], inv_dam [j], inv_plus [j], inv_quant [j], inv_ident [j], 3));


                        //item_name(inv_class [j], inv_type [j], inv_dam [j], inv_plus [j], inv_quant [j], inv_ident [j], 3, st_pass);

        //strcpy(st_pass, "a spell");

                        //strcpy(st_pass, "Hello!");

                        //cout << spell_name(spells [j]);

                        //cprintf(spell_name(spells [j]), st_pass);

                        spell_name(spells [j], st_pass);

                        cprintf(st_pass);

                        //cprintf("sdasd");

                        gotoxy(35, wherey());









         char sval [2];

         itoa((int) spell_value(spells [j]), sval, 10);
         cprintf(sval);


                // do this later:
        //cout << (int) spell_value(spells [j]);













                        //cprintf(st_pass);



//                      if (inv_quant [j] != 1 && inv_class [j] != 6 && inv_class [j] != 8)
//                              strcat(info [info_lines], "s");

                        /*
                        if (j == item_wielded) cprintf(" (weapon)");
                        if (j == body_armour | j == shield_armour | j == cloak_armour | j == head_armour | j == hand_armour | j == foot_armour) cprintf(" (worn)");
                        if (j == ring [0]) cprintf(" (left hand)");
                        if (j == ring [1]) cprintf(" (right hand)");
                        */
//                      cprintf("\n\r");

                       //       cprintf("\n\r");

                        //info_lines++;

                }
        } // end of j loop
        //} // end of if inv_class2
//} // end of i loop.
//}
/*else
        {
        if (item_class_inv == -1) cprintf("You aren't carrying anything.");
        else
        {
        if (item_class_inv == 0) cprintf("You aren't carrying any weapons.");
         else
         {
                if (item_class_inv == 1) cprintf("You aren't carrying any ammunition.");
                 else
                 {
                        cprintf("You aren't carrying any such object.");
                 }
         }
        anything++;
        }
        //info_lines ++;
        }*/
//} // end of if (inv_no)
//relay_message();
//more();

//} // end of inventory()


        //char ki;

   if (anything > 0)
   {
           ki = getch();
           //ki = getch();
           //ki = anything;
           if (ki >= 65 && ki < 123)
           {
                   puttext(35, 1, 80, 25, buffer);
                   return ki;
           }
           if (ki == 0) ki = getch();
           puttext(35, 1, 80, 25, buffer);
           return anything;
   }

  /*
   gotoxy(1, 25);
   cprintf("Press any key to clear screen...");
   getch();
   clrscr();
   gotoxy(1, 25);
   cprintf("Press any key to restore screen...");
   getch();
   */
   puttext(35, 1, 80, 25, buffer);

   ki = getch();

//   gotoxy(80,25);
//   cprintf(" \r");

//   gotoxy(1, 25);
//   cprintf("Press any key to quit...");
//   getch();

   return ki;


}





char spell_value(char spell)
{

switch(spell)
{
        case 0: return 6; // identify
        case 1: return 5; // teleportation
        case 2: return 4; // cause fear
        case 3: return 1; // noise
        case 4: return 5; // remove curse
        case 5: return 1; // magic missile
        case 6: return 5; // fireball
        case 13: return 3; // conjure flame
   case 14: return 4; // dig
   case 15: return 5; // firebolt
   case 16: return 5; // bolt of freezing cold
   case 17: return 6; // bolt of lightning
   case 20: return 6; // polymorph other
   case 21: return 3;
   case 22: return 4;
   case 23: return 4;
   case 24: return 3;
   case 25: return 4; // invisibility
   case 26: return 2; // throw flame
   case 27: return 2; // throw frost
   case 28: return 4; // blink
   case 29: return 7; // freezing cloud
   case 30: return 3; // stinking cloud
   case 31: return 8; // ring of flames
   case 32: return 2; //  restore strength:
   case 33: return 2; //          int
   case 34: return 2; //          dex
   case 35: return 5; // venom bolt
   case 36: return 4; // toxic radiance - uses lots of food?
   case 37: return 5; // teleport other
   case 38: return 2; // lesser healing
   case 39: return 6; // greater healing
   case 40: return 3; // cure poison
   case 41: return 5; // purification
   case 42: return 8; // death's door
   case 43: return 3; // selective amnesia
   case 44: return 7; // mass confusion
   case 45: return 4; // smiting
   case 46: return 3; // repel undead
   case 47: return 7; // holy word
   case 48: return 3; // detect curse
   case 49: return 1; // summon small mammal
   case 50: return 4; // Abjuration
   case 51: return 4; // summon scorpions
   case 52: return 3; // levitation
   case 53: return 6; // bolt of draining
   case 54: return 8; // splinters
   case 55: return 4; // innacuracy
   case 56: return 6; // poisonous cloud
   case 57: return 8; // fire storm
   case 58: return 2; // detect traps
   case 59: return 2; // random blink
   case 60: return 5; // Isk's blast
   case 61: return 5; // swarm
   case 62: return 8; // Summon Thing
   case 63: return 4; // Enslavement
   case 64: return 4; // Magic Mapping
   case 65: return 3; // heal other
default: return 2;












}

return 1;

}


void spell_name(char spell, char spln [60])
{
        //char spln [30] = "";

        strcpy(spln, "Hello!");

        switch(spell)
        {
                case 0: strcpy(spln, "Identify"); break;
                case 1: strcpy(spln, "Teleport Self"); break;
                case 2: strcpy(spln, "Cause Fear"); break;
                case 3: strcpy(spln, "Create Noise"); break;
                case 4: strcpy(spln, "Remove Curse"); break;
                case 5: strcpy(spln, "Magic Dart"); break;
                case 6: strcpy(spln, "Fireball"); break;
                case 13: strcpy(spln, "Conjure Flame"); break;
      case 14: strcpy(spln, "Dig"); break;
      case 15: strcpy(spln, "Bolt of Fire"); break;
      case 16: strcpy(spln, "Bolt of Cold"); break;
      case 17: strcpy(spln, "Thunderbolt"); break;
      case 20: strcpy(spln, "Polymorph Other"); break;
      case 21: strcpy(spln, "Slow"); break;
      case 22: strcpy(spln, "Haste"); break;
      case 23: strcpy(spln, "Paralyze"); break;
      case 24: strcpy(spln, "Confuse"); break;
      case 25: strcpy(spln, "Invisibility"); break;
      case 26: strcpy(spln, "Throw Flame"); break;
      case 27: strcpy(spln, "Throw Frost"); break;
      case 28: strcpy(spln, "Controlled Blink"); break;
      case 29: strcpy(spln, "Freezing Cloud"); break;
      case 30: strcpy(spln, "Mephitic Cloud"); break;
      case 31: strcpy(spln, "Ring of Flames"); break;
      case 32: strcpy(spln, "Restore Strength"); break;
      case 33: strcpy(spln, "Restore Intelligence"); break;
      case 34: strcpy(spln, "Restore Dexterity"); break;
      case 35: strcpy(spln, "Venom Bolt"); break;
      case 36: strcpy(spln, "Olgreb's Toxic Radiance"); break;
      case 37: strcpy(spln, "Teleport Other"); break;
      case 38: strcpy(spln, "Lesser Healing"); break;
      case 39: strcpy(spln, "Greater Healing"); break;
      case 40: strcpy(spln, "Cure Poison"); break;
      case 41: strcpy(spln, "Purification"); break;
      case 42: strcpy(spln, "Death's Door"); break;
      case 43: strcpy(spln, "Selective Amnesia"); break;
      case 44: strcpy(spln, "Mass Confusion"); break;
      case 45: strcpy(spln, "Smiting"); break;
                                                case 46: strcpy(spln, "Repel Undead"); break;
                                                case 47: strcpy(spln, "Holy Word"); break;
      case 48: strcpy(spln, "Detect Curse"); break;
      case 49: strcpy(spln, "Summon Small Mammal"); break;
      case 50: strcpy(spln, "Abjuration"); break;
      case 51: strcpy(spln, "Summon Scorpions"); break;
      case 52: strcpy(spln, "Levitation"); break;
      case 53: strcpy(spln, "Bolt of Draining"); break;
      case 54: strcpy(spln, "Throw Splinters"); break;
      case 55: strcpy(spln, "Bolt of Inaccuracy"); break;
                                          // spelling!
      case 56: strcpy(spln, "Poisonous Cloud"); break;
      case 57: strcpy(spln, "Fire Storm"); break;
      case 58: strcpy(spln, "Detect Traps"); break;
      case 59: strcpy(spln, "Blink"); break;
      case 60: strcpy(spln, "Iskenderun's Mystic Blast"); break;
      case 61: strcpy(spln, "Swarm"); break;
      case 62: strcpy(spln, "Create Horrible Things"); break;
      case 63: strcpy(spln, "Enslavement"); break;
      case 64: strcpy(spln, "Magic Mapping"); break;
      case 65: strcpy(spln, "Heal Other"); break;
default: strcpy(spln, "another spell"); break;
        }

// purify food?

        //itoa(spell, spln, 10);

//return spln;

}


char spellbook_contents(unsigned char plus, unsigned char type)
{

char st_pass [60];

char stringy [9];
int numby = plus;
int j;
int spelcount = 0;
char strng [2] = "";
char ft = 0;

//char st_pass [60];



char buffer[2400];

   strcpy(st_pass, "");

  gettext(35, 1, 80, 25, buffer);

  strcpy(st_pass, "");

  window(35, 1, 80, 25);

  clrscr();


   int spell_types [10];

   spellbook_template(type, spell_types);

//   numby -= 64;

   itoa(numby, stringy, 2);


   for (j = 1; j < 7; j ++)
   {

      if (stringy [j] != 49) continue;

                                cprintf(" ");
                                if (spelcount < 26) ft = (char) spelcount + 97;
                                        else ft = (char) spelcount + 65;

               strcpy(strng, "");

                                strng [0] = ft;
            strng [1] = 0;

                                cprintf(strng);

                        cprintf(" - ");

//                      spell_name(???, st_pass);

//                      cprintf(st_pass);

                        spell_name(spell_types [j], st_pass);

                        cprintf(st_pass);

         //cprintf("A spell");

                        gotoxy(35, wherey());

         char sval [2];

         itoa((int) spell_value(spell_types [j]), sval, 10);
         cprintf(sval);


         cprintf("\n\r");

   spelcount ++;


   } // end of for j




   /*
   for (i = 0; i < 9; i ++)
   {
      stringy [i] += 48;

   }*/


   //cprintf(stringy);

   //getch();


//   goto inpy;


//if (getch() == 0) getch();

char keyn = getch();
if (keyn == 0) getch();

puttext(35, 1, 80, 25, buffer);

return keyn;

}




int which_spell_in_book(int sbook_type, int spl)
{
int ret = -1;
int func_pass [10];

spellbook_template(sbook_type, func_pass);

return func_pass [spl];



}

int spell_hunger(int spell_value, char spell)
{

switch(spell_value)
{
 case 1: return 50;
 case 2: return 95;
 case 3: return 180;
 case 4: return 300;
 case 5: return 400;
 case 6: return 600;
 case 7: return 850;
 case 8: return 1100;
 case 9: return 1600;
}

} // end of spell_hunger


/*
        case 0: strcpy(spln, "Identify"); break;
                case 1: strcpy(spln, "Teleport Self"); break;
                case 2: strcpy(spln, "Cause Fear"); break;
                case 3: strcpy(spln, "Create Noise"); break;
                case 4: strcpy(spln, "Remove Curse"); break;
                case 5: strcpy(spln, "Magic Missile"); break;
                case 6: strcpy(spln, "Fireball"); break;
                case 13: strcpy(spln, "Conjure Flame"); break;
      case 14: strcpy(spln, "Dig"); break;
      case 15: strcpy(spln, "Bolt of Fire"); break;
      case 16: strcpy(spln, "Bolt of Cold"); break;
      case 17: strcpy(spln, "Thunderbolt"); break;
      case 20: strcpy(spln, "Polymorph Other"); break;
      case 21: strcpy(spln, "Slow"); break;
      case 22: strcpy(spln, "Haste"); break;
      case 23: strcpy(spln, "Paralyze"); break;
      case 24: strcpy(spln, "Confuse"); break;
      case 25: strcpy(spln, "Invisibility"); break;
      case 26: strcpy(spln, "Throw Flame"); break;
      case 27: strcpy(spln, "Throw Frost"); break;
      case 28: strcpy(spln, "Controlled Blink"); break;
      case 29: strcpy(spln, "Freezing Cloud"); break;
      case 30: strcpy(spln, "Mephitic Cloud"); break;
      case 31: strcpy(spln, "Ring of Flames"); break;
      case 32: strcpy(spln, "Restore Strength"); break;
      case 33: strcpy(spln, "Restore Intelligence"); break;
      case 34: strcpy(spln, "Restore Dexterity"); break;
      case 35: strcpy(spln, "Venom Bolt"); break;
      case 36: strcpy(spln, "Olgreb's Toxic Radiance"); break;
      case 37: strcpy(spln, "Teleport Creature"); break;
      case 38: strcpy(spln, "Lesser Healing"); break;
      case 39: strcpy(spln, "Greater Healing"); break;
      case 40: strcpy(spln, "Cure Poison"); break;
      case 41: strcpy(spln, "Purification"); break;
      case 42: strcpy(spln, "Death's Door"); break;
      case 43: strcpy(spln, "Selective Amnesia"); break;
      case 44: strcpy(spln, "Mass Confusion"); break;
      case 45: strcpy(spln, "Smiting"); break;
                                                case 46: strcpy(spln, "Repel Undead"); break;
                                                case 47: strcpy(spln, "Holy Word"); break;
      case 48: strcpy(spln, "Detect Curse"); break;
      case 49: strcpy(spln, "Summon Small Mammal"); break;
      case 50: strcpy(spln, "Abjuration"); break;
      case 51: strcpy(spln, "Summon Scorpions"); break;
      case 52: strcpy(spln, "Levitation"); break;
      case 53: strcpy(spln, "Bolt of Draining"); break;
      case 54: strcpy(spln, "Throw Splinters"); break;
      case 55: strcpy(spln, "Bolt of Inaccuracy"); break;
                                          // spelling!
      case 56: strcpy(spln, "Poisonous Cloud"); break;
      case 57: strcpy(spln, "Fire Storm"); break;
      case 58: strcpy(spln, "Detect Traps"); break;
      case 59: strcpy(spln, "Blink"); break;
      case 60: strcpy(spln, "Iskenderun's Mystic Blast"); break;
      case 61: strcpy(spln, "Create Swarm"); break;
      case 62: strcpy(spln, "Create Horrible Thing");
      case 63: enslavement
      case 64: magic mapping
*/
