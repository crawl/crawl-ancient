#include "config.h"
/* Message File */

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include <string.h>

void more(void);

char scrloc = 1; /* Line of next (previous?) message */

char store_message [30] [200]; /* buffer of old messages */
char store_count = 0; /* current position in store_message */


void mpr(char inf [200])
{
char inf_screens = 0;

char info2 [80];
you[0].running = 0;


#ifdef DOS
window(1, 1, 80, 25);
#endif


textcolor(7);

//if (scrloc == 8)
if (scrloc == 7)
{
#ifdef LINUX
gotoxy(2,25);
_setcursortype(_NORMALCURSOR);
textcolor(7);
cprintf("\r--more--");
char keypress = 0;
do
{
        keypress = getch();
} while (keypress != 32 && keypress != 13);
char del_line_no = 0;
gotoxy(1, 18);
//for (del_line_no = 0; del_line_no < 8; del_line_no ++)
for (del_line_no = 0; del_line_no < 7; del_line_no ++)
{
 cprintf("                                                                               ");
// if (del_line_no != 7) cprintf("\n\r");
 if (del_line_no != 6) cprintf("\n\r");
}
#endif

#ifdef DOS
 window(1, 18, 78, 25);
 more();
 window(1, 1, 80, 25);
#endif
 scrloc = 0;
}

//gotoxy(1, scrloc + 17);
gotoxy(1, scrloc + 18);
strncpy(info2, inf, 78);
cprintf(info2);

/* Put the message into store_message, and move the '---' line forward */
strncpy(store_message [store_count], inf, 78);
store_count ++;
if (store_count > 23) store_count -= 24;
strcpy(store_message [store_count], "------------------------------------------------------------------------------");

inf_screens = 0;

scrloc ++;

} // end of message function

void mesclr(void)
{
 _setcursortype(_NOCURSOR);
#ifdef DOS
 window(1,18,78,25);
 clrscr();
 window(1, 1, 80, 25);
#endif

#ifdef LINUX
char del_line_no = 0;
gotoxy(1, 18);
for (del_line_no = 0; del_line_no < 8; del_line_no ++)
{
 cprintf("                                                                               ");
 if (del_line_no != 7) cprintf("\n\r");
}
#endif
 scrloc = 0;
 _setcursortype(_NORMALCURSOR);
 gotoxy(18,9);
}


void more(void)
{
char keypress = 0;

#ifdef LINUX
 gotoxy(2,25);
#endif
#ifdef DOS
 window(1, 18, 80, 25);
 gotoxy(2, 8);
#endif
 _setcursortype(_NORMALCURSOR);
 textcolor(7);
 cprintf("\r--more--");
 do
 {
        keypress = getch();
 } while (keypress != 32 && keypress != 13);

 keypress = 0;
#ifdef DOS
 window(1, 18, 80, 25);
#endif
 clrscr();
 scrloc = 0;

}






void replay_messages(void)
{
 _setcursortype(_NOCURSOR);

   int i = 0;
   int j= 0;
   int line = 0;
#ifdef DOS
   char buffer[4800];
   window(1, 1, 80, 25);
   gettext(1, 1, 80, 25, buffer);
#endif

   clrscr();

   gotoxy(1,1);

   line = 0;

   for (j = 0; j < 24; j ++)
   {
    if (strncmp(store_message [j], "--------------", 10) == 0)
    {
     line = j;
     break;
    }
   }

i = line + 1;
if (i == 24) i = 0;

do
{
 cprintf(store_message [i]);
 cprintf("\n\r");
 if (i == line) break;
 i ++;
 if (i == 24) i = 0;
} while (1);

/*
                        for (i = 0; i < 24; i ++)
                        {
                                cprintf(store_message [i]);
                                cprintf("\n\r");
                        }
*/
   if (getch() == 0) getch();


#ifdef DOS
   puttext(1, 1, 80, 25, buffer);
#endif

 _setcursortype(_NORMALCURSOR);

   return;

}
