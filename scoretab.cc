#define DOS

#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DOS
#include <file.h>
#include <conio.h>
#include <dos.h>
#endif
#ifdef LINUX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif


int main(void) //int argc, char *argv[])
{

char signat [80];
char getsig [80];

/*if (argc > 1)
{
 if (strlen(argv [1]) > 39) argv [1] [40] = 0;
} else
{
 cprintf("\n\rPlease enter a name.\n\r");
 return 1;
}
*/
cprintf("This is the Crawl top 100 collating program.\n\r");
cprintf("Run it in the same directory as a Crawl scores file (of any version), and it\n\r");
cprintf("will add the contents of that score file into the hallfame.txt file. The same\n\r");
cprintf("score file can be run through multiple times, as duplicate entries are ignored.\n\r");
cprintf("Each new entry is stamped with today's date, and a name.\n\r\n\r");

cprintf("\n\rPlease enter name (blank to quit): ");
gets(getsig);

if (strlen(getsig) == 0)
{
 cprintf("\n\rBye bye.");
 return 0;
}

cprintf("\n\r");

if (strlen(getsig) > 39) getsig [40] = 0;

strcpy(signat, "         - ");
strcat(signat, getsig);
strcat(signat, ", (");

/*struct dates {
short da_year;
char da_day;
char da_mon;
};

struct dates retdate [1];*/

struct date retdate [1];

getdate(retdate);

char st_prn [5];

itoa(retdate[0].da_day, st_prn, 10);
strcat(signat, st_prn);
strcat(signat, "/");
itoa(retdate[0].da_mon, st_prn, 10);
strcat(signat, st_prn);
strcat(signat, "/");
itoa(retdate[0].da_year, st_prn, 10);
strcat(signat, st_prn);



strcat(signat, ")");

cprintf("Signature: ");
cprintf(signat);
cprintf("\n\r");


char high_scores [20] [80];
long scores [20];

char top_100 [100] [81];
long scores_100 [100];

char signatur [100] [81];

int hc = 0;
int hc2 = 0;
int hc3 = 0;
char ready [81];
int multip = 1;
int handle2;


for (hc2 = strlen(signat); hc2 < 80; hc2 ++)
{
  signat [hc2] = ' ';
}



ready [0] = 0;
ready [1] = 0;

/*for (hc2 = strlen(death_string); hc2 < 80; hc2 ++)
{
 death_string [hc2] = ' ';
}*/

for (hc = 0; hc < 20; hc ++)
{
 strcpy(high_scores [hc], "-empty");
 scores [hc] = 0;
}

for (hc = 0; hc < 100; hc ++)
{
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  top_100 [hc] [hc2] = 32;
  signatur [hc] [hc2] = 32;
 }
 top_100 [hc] [80] = 0;
 signatur [hc] [80] = 0;
}

for (hc = 0; hc < 100; hc ++)
{
 strcpy(top_100 [hc], "-empty");
 scores_100 [hc] = 0;
}

cprintf("Opening Scores file...\n\r");

int handle = open("scores", S_IWRITE, S_IREAD);

if (handle == -1)
{
for (hc = 0; hc < 15; hc ++)
{
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  high_scores [hc] [hc2] = 32;
 }
   strcpy(high_scores [hc], "0       empty");
   scores [hc] = 0;

}
cprintf("Scores file not found.\n\r");
goto out_of_ready;
}


for (hc = 0; hc < 15; hc ++)
{
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  read(handle, ready, 1);
/*  if (ready [0] == 13 | ready [0] == 0 | ready [0] == '\r' | ready [0] == '\n')
  {
   for (hc3 = hc2; hc3 < 80; hc3 ++)
   {
    high_scores [hc] [hc3] = 32;
    goto out_of_inner;
   }
  }*/
  if (ready [0] == EOF) goto out_of_ready;
  if (ready [0] == 0 | ready [0] == 7) ready [0] = 32;
  high_scores [hc] [hc2] = ready [0];
 } // end for hc2
out_of_inner : hc3 = 0;
} // end for hc

for (hc = 0; hc < 15; hc ++)
{
 multip = 1;
 for (hc2 = 6; hc2 >= 0; hc2 --)
 {
  if (high_scores [hc] [hc2] == 32) continue;
  scores [hc] += (high_scores [hc] [hc2] - 48) * multip;
  multip *= 10;
 }
}

out_of_ready :

cprintf("Opening Hall of Fame file for reading...\n\r");

handle2 = open("hallfame.txt", S_IWRITE, S_IREAD);

if (handle2 == -1)
{
for (hc = 0; hc < 100; hc ++)
{
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  top_100 [hc] [hc2] = 32;
 }
//   strcpy(top_100 [hc], "0       empty");
   scores_100 [hc] = 0;
   top_100 [hc] [0] = '0';

}
cprintf("Hall of Fame file not found, creating.\n\r");
goto out_of_ready2;
}

for (hc = 0; hc < 100; hc ++)
{
 read(handle2, ready, 3);
 for (hc2 = 0; hc2 < 77; hc2 ++)
 {
  read(handle2, ready, 1);
/*  if (ready [0] == 13 | ready [0] == 0 | ready [0] == '\r' | ready [0] == '\n')
  {
   for (hc3 = hc2; hc3 < 80; hc3 ++)
   {
    top_100 [hc] [hc3] = 32;
    goto out_of_inner2;
   }
  }*/
  if (ready [0] == EOF) goto out_of_ready2;
  if (ready [0] == 0 | ready [0] == 7) ready [0] = 32;
  top_100 [hc] [hc2] = ready [0];
 } // end for hc2
out_of_inner2 : hc3 = 0;

 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  read(handle2, ready, 1);
  if (ready [0] == 13 | ready [0] == 0 | ready [0] == '\r' | ready [0] == '\n')
  {
   for (hc3 = hc2; hc3 < 80; hc3 ++)
   {
    signatur [hc] [hc3] = 32;
    goto out_of_inner3;
   }
  }
  if (ready [0] == EOF) goto out_of_ready2;
  if (ready [0] == 0 | ready [0] == 7) ready [0] = 32;
  signatur [hc] [hc2] = ready [0];
 } // end for hc2
out_of_inner3 : hc3 = 0;



} // end for hc

for (hc = 0; hc < 100; hc ++)
{
 multip = 1;
 for (hc2 = 6; hc2 >= 0; hc2 --)
 {
  if (top_100 [hc] [hc2] == 32) continue;
  scores_100 [hc] += (top_100 [hc] [hc2] - 48) * multip;
  multip *= 10;
 }
}

out_of_ready2 :

/*for (hc = 0; hc < 100; hc ++)
{
 for (hc2 = strlen(top_100 [hc]); hc2 < 80; hc2 ++)
 {
  top_100 [hc] [hc2] = ' ';
 }
}*/


int placed = 0;
char has_printed = 0;

int i = 0;

/*for (i = 0; i < 15; i ++
{
for (hc = 0; hc < 100; hc ++)
{
 placed ++;
if (scores [i] > top_100 [hc] && has_printed == 0)
{
 textcolor(YELLOW);
  itoa(placed, ready, 10);
  cprintf(ready);
  if (strlen(ready) == 1) cprintf("- "); else cprintf("-");
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  if (death_string [hc2] == 32 && death_string [hc2 + 1] == 32 && hc2 > 15)
  {
   cprintf("\n\r");
   break;
  }
  putch(death_string [hc2]);
 }
 has_printed = 1;
 hc --;
 textcolor(LIGHTGREY);
 continue;
}
  itoa(placed, ready, 10);
  cprintf(ready);
  if (strlen(ready) == 1) cprintf("- "); else cprintf("-");
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  if (high_scores [hc] [hc2] == 32 && high_scores [hc] [hc2 + 1] == 32 && hc2 > 15)
  {
   cprintf("\n\r");
   break;
  }
  putch(high_scores [hc] [hc2]);
 }
}
} // end for i*/

close(handle);
close(handle2);


cprintf("Opening Hall of Fame file for writing...\n\r");

handle = open("hallfame.txt", O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0660);


if (handle == -1)
{
        perror("oh NO!!!");
//      if (getch() == 0) getch();
        return 0;
}

/*for (hc = 0; hc < 100; hc ++)
{
 for (hc2 = strlen(top_100 [hc]); hc2 < 80; hc2 ++)
 {
  top_100 [hc] [hc2] = ' ';
 }
}*/

for (hc = 0; hc < 100; hc ++)
{
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  if (top_100 [hc] [hc2] == '\n' | top_100 [hc] [hc2] == '\r')
   top_100 [hc] [hc2] = 32;
  if (signatur [hc] [hc2] == '\n' | signatur [hc] [hc2] == '\r')
   signatur [hc] [hc2] = 32;
 }
}


//char score_comp [80];

for (hc2 = 0; hc2 < 15; hc2 ++)
{
 for (hc = 0; hc < 99; hc ++)
 {
//  if (strstr(high_scores [hc2], top_100 [hc]) != NULL)
  if (strncmp(high_scores [hc2], top_100 [hc], 21) == 0)
  {
   scores [hc2] = 0;
   i ++;
   break;
  }
 }
}

cprintf("Found ");
itoa(i, st_prn, 10);
cprintf(st_prn);
cprintf(" duplicate scores.\n\r");

char printed = 1;
i = 0;

cprintf("Commencing collation process (hit a key to continue)\n\r");
if (getch() == 0) getch();

for (hc = 0; hc < 99; hc ++)
{
 if (printed == 99) break;
 itoa(printed, st_prn, 10);
 if (st_prn [1] == 0)
 {
  st_prn [1] = ' ';
 }
 st_prn [2] = '-';
 write(handle, st_prn, 3);

 while (scores [i] == 0)
 {
  i++;
 };

 if (i <= 14 && scores [i] > scores_100 [hc] && strstr(top_100 [hc], high_scores [i]) == NULL)
 {
  write(handle, high_scores [i], 76);
  write(handle, "\n", 1);
  hc --;
  i ++;
  write(handle, signat, 79);
  write(handle, "\n", 1);
  cprintf("Adding score ");
  itoa(i, st_prn, 10);
  cprintf(st_prn);
  cprintf(" before ");
  itoa(hc, st_prn, 10);
  cprintf(st_prn);
  cprintf(".\n\r");
 } else
 {
  write(handle, top_100 [hc], 76);
  write(handle, "\n", 1);
  write(handle, signatur [hc], 79);
  write(handle, "\n", 1);
 }
 printed ++;
}

close(handle);

return 1;

} // end void highscores
