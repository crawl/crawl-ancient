#include "config.h"

#include "externs.h"
#include "monplace.h"
#include "mstruct.h"
#include "stuff.h"

void init_pandemonium(void)
{

int pc = 0;
for (pc = 0; pc < MNST; pc ++)
{
 // this looks for special (unique) demons and sets appropriate lists of demons. Note that this also sets the level colours.
 if (menv [pc].m_class == 251) // Nemelex
 {
  env[0].mons_alloc [0] = 23;
  env[0].mons_alloc [1] = 23;
  env[0].mons_alloc [2] = 23;
  env[0].mons_alloc [3] = 49;
  env[0].mons_alloc [4] = 225;
  env[0].mons_alloc [5] = 224;
  env[0].mons_alloc [6] = 225;
  env[0].mons_alloc [7] = 232;
  env[0].mons_alloc [8] = 233;
  env[0].mons_alloc [9] = 221;
  return;
 }

 if (menv [pc].m_class == 252) // Sif Muna
 {
  env[0].mons_alloc [0] = 227;
  env[0].mons_alloc [1] = 228;
  env[0].mons_alloc [2] = 228;
  env[0].mons_alloc [3] = 229;
  env[0].mons_alloc [4] = 231;
  env[0].mons_alloc [5] = 232;
  env[0].mons_alloc [6] = 228;
  env[0].mons_alloc [7] = 227;
  env[0].mons_alloc [8] = 220;
  env[0].mons_alloc [9] = 227;
  return;
 }

 if (menv [pc].m_class == 253) // Okawaru
 {
  env[0].mons_alloc [0] = 68; // Efreet
  env[0].mons_alloc [1] = 23;
  env[0].mons_alloc [2] = 226;
  env[0].mons_alloc [3] = 226;
  env[0].mons_alloc [4] = 225;
  env[0].mons_alloc [5] = 221;
  env[0].mons_alloc [6] = 226;
  env[0].mons_alloc [7] = 229;
  env[0].mons_alloc [8] = 233;
  env[0].mons_alloc [9] = 233;
  return;
 }

 if (menv [pc].m_class == 254) // Kikubaaqudgha
 {
  env[0].mons_alloc [0] = 107;
  env[0].mons_alloc [1] = 107;
  env[0].mons_alloc [2] = 108;
  env[0].mons_alloc [3] = 220;
  env[0].mons_alloc [4] = 234;
  env[0].mons_alloc [5] = 227;
  env[0].mons_alloc [6] = 228;
  env[0].mons_alloc [7] = 230;
  env[0].mons_alloc [8] = 230;
  env[0].mons_alloc [9] = 230;
  return;
 }

 if (menv [pc].m_class == 255) // mollusc lord
 {
  env[0].mons_alloc [0] = 242;
  env[0].mons_alloc [1] = 23;
  env[0].mons_alloc [2] = 23;
  env[0].mons_alloc [3] = 242;
  env[0].mons_alloc [4] = 243;
  env[0].mons_alloc [5] = 242;
  env[0].mons_alloc [6] = 243;
  env[0].mons_alloc [7] = 49;
  env[0].mons_alloc [8] = 49;
  env[0].mons_alloc [9] = 49;
  return;
 }

}
// colour of monster 9 is colour of floor, 8 is colour of rock
//  IIRC, BLACK is set to LIGHTGRAY


for (pc = 0; pc < 10; pc ++)
{

switch(random2(17))
{
 case 10:
 case 0: env[0].mons_alloc [pc] = 220; break;
 case 11:
 case 1: env[0].mons_alloc [pc] = 221; break;
 case 12:
 case 2: env[0].mons_alloc [pc] = 222; break;
 case 13:
 case 3: env[0].mons_alloc [pc] = 223; break;
 case 14:
 case 4: env[0].mons_alloc [pc] = 224; break;

 case 5: env[0].mons_alloc [pc] = 225; break;
 case 6: env[0].mons_alloc [pc] = 226; break;
 case 7: env[0].mons_alloc [pc] = 227; break;
 case 8: env[0].mons_alloc [pc] = 228; break;
 case 9: env[0].mons_alloc [pc] = 229; break;

 case 15: env[0].mons_alloc [pc] = 23; break;
 case 16: env[0].mons_alloc [pc] = 49; break;
}

    if (random2(10) == 0) env[0].mons_alloc [pc] = 80 + random2(10);
    if (random2(30) == 0) env[0].mons_alloc [pc] = 3;
    if (random2(30) == 0) env[0].mons_alloc [pc] = 8;
    if (random2(30) == 0) env[0].mons_alloc [pc] = 235 + random2(3);

}

if (random2(8) == 0) env[0].mons_alloc [7] = 230 + random2(5);
if (random2(5) == 0) env[0].mons_alloc [8] = 230 + random2(5);
if (random2(3) == 0) env[0].mons_alloc [9] = 230 + random2(5);

   if (random2(10) == 0) env[0].mons_alloc [7 + random2(3)] = 31;
   if (random2(10) == 0) env[0].mons_alloc [7 + random2(3)] = 126;
   if (random2(10) == 0) env[0].mons_alloc [7 + random2(3)] = 127;
   if (random2(10) == 0) env[0].mons_alloc [7 + random2(3)] = 245;

// set at least some specific monsters for the special levels - this
//  can also be used to set some colours

}

void pandemonium_mons(void)
{

// must leave allowance for monsters rare on pandemonium (eg wizards etc)

int pan_mons = env[0].mons_alloc [random2(10)];

if (random2(40) == 0)
{
 do
 {
  pan_mons = random2(400);
 } while (mons_pan(pan_mons) == 0);
}

mons_place(pan_mons, 0, 50, 50, 0, MHITNOT, 250, 52);

}
