#define MNST 200
//#include <string.h>


void monster_grid(unsigned char no_mons, unsigned char mons_x [MNST], unsigned char mons_y [MNST], unsigned char mons_beh [MNST], int invis, char see_invis, int mons_class [MNST], char mcolour [MNST], int x_pos, int y_pos, unsigned char show [15] [15], unsigned char show_col [15] [15], char mons_ench [MNST] [3], int running [10], int mons_sec [MNST])
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
        } else running [0] = 0;

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


} // end of monster_grid()
