//#include <alloc.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>

char is_a_vowel(unsigned char let);
char retvow(char sed);
char retbit(char sed);
char reduce(unsigned char reducee);

char *retcons(char seeds);






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
