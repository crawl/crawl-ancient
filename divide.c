#include <conio.h>
#include <stdlib.h>

int globo(unsigned int glob2);

int main(void)
{
 globo(-10);
 return 0;
}

int globo(unsigned int glob2)
{

 int plob = random() % glob2;

 char string [10];
 itoa(plob, string, 10);
 cprintf(string);
 cprintf("\n\r");


}
