#ifndef NEWMONST_H
#define NEWMONST_H


void monster(void)
{
    char bat = 0;
    int monc = 0;

    int msecc = 0;
    char brkk = 0;

//cprintf("og");




//itoa((int) time_taken, st_prn, 10);
    //cprintf(st_prn);

//getch();
    //delay(500);

    brek = 0;
//viewwindow(); // only once; before the monster function
    losight(show, grid, you[0].x_pos, you[0].y_pos);

    brek = 0;
//mnc = 0;



//viewwindow(); // only once; before the monster function


//itoa(no_mons, st_prn, 10);
    //mons [i].m_speed, st_prn, 10);
    //strcpy(info, st_prn);
    //strcat(info, ",");
    //itoa(mons [i].m_speed_inc, st_prn, 10);
    //strcat(info, st_prn);
    //incrl();

    no_mons = 0;

    for (i = 0; i < MNST; i++)
    {
/*
   if (mons_near(i) == 9)
   {
   itoa(mnc, st_prn, 10);
   //mons [i].m_speed, st_prn, 10);
   strcpy(info, st_prn);
   //strcat(info, ",");
   //itoa(mons [i].m_speed_inc, st_prn, 10);
   //strcat(info, st_prn);
   incrl();
   }
 */

//if (mons_class [i] == -1) continue;


//if (monc > no_mons) break;





/*
   if (mons_near(i) == 1)
   {
   itoa(mnc, st_prn, 10);
   //mons [i].m_speed, st_prn, 10);
   strcpy(info, st_prn);
   //strcat(info, ",");
   //itoa(mons [i].m_speed_inc, st_prn, 10);
   //strcat(info, st_prn);
   incrl();
   }
 */

/*
   if (mons_near(i))
   {
   strcpy(info, "z");
   incrl();
   }
 */

        if (mons[i].m_class != -1)
            no_mons++;

//if (mons_class [i] == -1) continue; // if monster isn't there, don't bother.
        if (mons[i].m_class != -1 && mons[i].m_class != 56)
        {

            monc++;
//{
            //grid [mons [i].m_x] [mons [i].m_y] = basic_grid [mons [i].m_x] [mons [i].m_y];
            //grid_col [mons [i].m_x] [mons [i].m_y] = basic_grid_col [mons [i].m_x] [mons [i].m_y];

            if (mons[i].m_class == 32 && mons[i].m_hp < 1)
            {
                for (j = 0; j < 3; j++)
                {
                    mons[i].m_ench[j] = 0;
                }

                mons[i].m_ench_1 = 0;
                //itc = 0;
                no_mons--;
                mons[i].m_class = -1;
//      if (grid [mons [i].m_x] [mons [i].m_y] >= 50) grid [mons [i].m_x] [mons [i].m_y] -= 50;
                mgrid[mons[i].m_x][mons[i].m_y] = MNG;
                spore_goes_pop();       // has to be after the above, so that spore isn't killed twice.

                continue;
            }                   // end of if giant spore



//if (mons [i].m_hp <= (mons [i].m_hp_max / 4 - 1)) mons [i].m_beh = 3;
            // move this ^^^

/*
   // enchantments:

   if (mons [i].m_ench_1 != 0)
   {

   for (p = 0; p < 3; p ++)
   {
   switch (mons [i].m_ench [p])
   {
   case 1: // you[0].slow
   if (random2(60) == 0)
   {
   mons [i].m_speed *= 2;
   mons [i].m_ench [p] = 0;
   if (mons [i].m_ench [0] == 0 && mons [i].m_ench [1] == 0 && mons [i].m_ench [2] == 0)
   mons [i].m_ench_1 = 0;
   }
   break;

   case 2: // you[0].haste
   if (random2(60) == 0)
   {
   mons [i].m_speed /= 2;
   mons [i].m_ench [p] = 0;
   if (mons [i].m_ench [0] == 0 && mons [i].m_ench [1] == 0 && mons [i].m_ench [2] == 0)
   mons [i].m_ench_1 = 0;
   }
   break;


   case 3: // you[0].paralysis
   //mons [i].m_targ_1_x = mons [i].m_x;
   //mons [i].m_targ_1_y = mons [i].m_y;
   //mons [i].m_beh = 0;
   //mons [i].m_speed += 100;
   if (random2(20) == 0)
   {
   mons [i].m_speed -= 100;
   //mons [i].m_ev += 5;
   mons [i].m_ench [p] = 0;
   //mons [i].m_speed /= 2;
   //mons [i].m_ench [p] = 0;
   //mons [i].m_beh =
   if (mons [i].m_ench [0] == 0 && mons [i].m_ench [1] == 0 && mons [i].m_ench [2] == 0)
   mons [i].m_ench_1 = 0;
   }
   break;


   case 4: // fear
   mons [i].m_beh = 3;
   if (random2 (40) == 0)
   {
   mons [i].m_beh = 1; // reset to monster's original behaviour
   mons [i].m_ench [p] = 0;
   if (mons [i].m_ench [0] == 0 && mons [i].m_ench [1] == 0 && mons [i].m_ench [2] == 0)
   mons [i].m_ench_1 = 0;
   }
   break;

   } // end of switch
   } // end of for p
   if (mons [i].m_speed >= 100) continue;
   } // end of if
 */


            if (you[0].haste == 0)
                mons[i].m_speed_inc += (mons[i].m_speed * time_taken / 10);
            else
                mons[i].m_speed_inc += (mons[i].m_speed * (time_taken / 2) / 10);

            if (you[0].slow > 0)
                mons[i].m_speed_inc += (mons[i].m_speed * time_taken / 10);

//if (mons [i].m_speed >= 100) mons [i].m_speed_inc -= 100;


// Note: Put regeneration before enchantments so that paralysed creatures still regenerate.

/*
   if (grid [mons [i].m_x] [mons [i].m_y] >= 100)
   {
   mons_in_cloud();
   // abort();
   if (mons_class [i] == -1)
   {
   mons [i].m_speed_inc = 1;
   break;
   }
   //   cout << "/" << (int) grid [mons [i].m_x] [mons [i].m_y] << "/";
   //   getch();
   }
 */



/*
   if (mons_near(i))
   {
   strcpy(info, "z");
   incrl();
   }
 */







            while (mons[i].m_speed_inc >= 80)   // The continues & breaks are WRT this.

            {

                if (mons[i].m_class != -1 && mons[i].m_hp <= 0)
                {
                    killer = 5;
                    monster_die(i);
                    if (mons[i].m_class != 32)
                        continue;
                }

                mons[i].m_speed_inc -= 10;

//abort();

                if (cgrid[mons[i].m_x][mons[i].m_y] != CNG)
                {
                    if (mons[i].m_class >= MLAVA0 && mons[i].m_sec == 1)
                        break;
                    if (mons[i].m_class == -1)
                        break;  // problem with vortices

                    mons_in_cloud();
                    if (mons[i].m_class == -1)
                    {
                        mons[i].m_speed_inc = 1;
                        break;
                    }
//      cout << "/" << (int) grid [mons [i].m_x] [mons [i].m_y] << "/";
                    //      getch();
                }







/*
   if (mons_near(i))
   {
   strcpy(info, "x");
   incrl();
   }
   //keyin = getch(); //cprintf("x");
 */




                if (you[0].invis != 0 && mons_see_invis(mons[i].m_class) != 1 && mons[i].m_targ_1_x == mons[i].m_x && mons[i].m_targ_1_y == mons[i].m_y)
                    mons[i].m_beh = 2;



//if (mons [i].m_beh == 0) continue; // if it hasn't found you yet.

                switch (mons[i].m_beh)
                {
                case 0:
                    mons[i].m_targ_1_x = mons[i].m_x;
                    mons[i].m_targ_1_y = mons[i].m_y;
                    break;

                case 100:
                case 1: // chasing YOU
                    //losight(show, grid, you[0].x_pos, you[0].y_pos);
                    //if ((mons_near(i) == 1 && you[0].invis == 0) | mons_class [i] == 19)// && random2 (6) != 0))

                    if ((mons_near(i) == 1 && (you[0].invis == 0 | mons_see_invis(mons[i].m_class) != 0)) | (mons_near(i) == 0 && mons[i].m_targ_1_x == mons[i].m_x && mons[i].m_targ_1_y == mons[i].m_y))
                    {
                        mons[i].m_targ_1_x = you[0].x_pos;
                        mons[i].m_targ_1_y = you[0].y_pos;
                    }
                    break;

                case 2:
                    if ((mons[i].m_targ_1_x == mons[i].m_x && mons[i].m_targ_1_y == mons[i].m_y) | random2(20) == 0)
                    {
                        mons[i].m_targ_1_x = random2(80);
                        mons[i].m_targ_1_y = random2(70);
                    }
                    break;      // wandering

                case 3: // fleeing

                    if (mons_near(i) == 1 && (you[0].invis == 0 | mons_see_invis(mons[i].m_class) > 0))
                    {
                        mons[i].m_targ_1_x = you[0].x_pos;
                        mons[i].m_targ_1_y = you[0].y_pos;
                    }
                    if (mons[i].m_hp > (mons[i].m_hp_max / 2))
                        mons[i].m_beh = 1;
// change the '= 1' to whatever monster used to be doing
                    break;

// 4 is used by confusion

                case 6: // attacking other monster

                    if (mons[i].m_hit == MHITNOT)
                    {
                        mons[i].m_beh = 2;      // ?

                        break;
                    }
                    mons[i].m_targ_1_x = mons[mons[i].m_hit].m_x;
                    mons[i].m_targ_1_y = mons[mons[i].m_hit].m_y;
                    break;

                case 7: // friendly
                    //To be nice (although not strictly accurate) they
                    //should follow you even when they can't see you.

                    if (you[0].pet_target != MHITNOT)
                    {
                        if (mons[i].m_hit == MHITNOT)
                        {
                            mons[i].m_hit = you[0].pet_target;
                        }
                    }

                    if (mons[i].m_hit == i | mons[mons[i].m_hit].m_beh == 7)
                        mons[i].m_hit = MHITNOT;
// I don't know why I have to do that.

                    if (mons[i].m_hit != MHITNOT && ((mons_near(i) == 1 && mons_near(mons[i].m_hit) == 1) | distance(mons[i].m_x, mons[mons[i].m_hit].m_x, mons[i].m_y, mons[mons[i].m_hit].m_y) == 1))
                    {
                        mons[i].m_targ_1_x = mons[mons[i].m_hit].m_x;
                        mons[i].m_targ_1_y = mons[mons[i].m_hit].m_y;
                    }
                    else if (mons_near(i) == 1 && (mons[i].m_targ_1_x == mons[i].m_x && mons[i].m_targ_1_y == mons[i].m_y) | random2(6) == 0)
                    {
                        if (random2(7) == 0)
                        {
                            mons[i].m_targ_1_x = random2(80);
                            mons[i].m_targ_1_y = random2(70);
                        }
                        else
                        {
                            mons[i].m_targ_1_x = you[0].x_pos;
                            mons[i].m_targ_1_y = you[0].y_pos;
                        }
                    }           /*else
                                   {
                                   mons [i].m_targ_1_x = you[0].x_pos;
                                   mons [i].m_targ_1_y = you[0].y_pos;
                                   } */
                    break;


/*
   case 4: //if ((mons [i].m_targ_1_x == mons [i].m_x && mons [i].m_targ_1_y == mons [i].m_y) | random2(50) == 0)
   mons [i].m_targ_1_x = random2(80);
   mons [i].m_targ_1_y = random2(70);
   break; // confused
 */
// A confused monster shouldn't reset its target - just move random2ly.




                }               // end of switch (mons_beh)




// enchantments:

                if (mons[i].m_ench_1 != 0)
                {

                    //mons_ench_f ( );

                    for (p = 0; p < 3; p++)
                    {
                        switch (mons[i].m_ench[p])
                        {
                        case 1: // you[0].slow

                            if (random2(250) <= mons[i].m_HD + 10)
                            {
                                if (mons[i].m_speed >= 100)
                                {
                                    mons[i].m_speed = ((mons[i].m_speed - 100) * 2) + 100;
                                }
                                else
                                    mons[i].m_speed *= 2;
                                mons[i].m_ench[p] = 0;
                                if (mons[i].m_ench[0] == 0 && mons[i].m_ench[1] == 0 && mons[i].m_ench[2] == 0)
                                    mons[i].m_ench_1 = 0;
                            }
                            break;

                        case 2: // you[0].haste

                            if (random2(40) == 0)
                            {
                                if (mons[i].m_speed >= 100)
                                {
                                    mons[i].m_speed = ((mons[i].m_speed - 100) / 2) + 100;
                                }
                                else
                                    mons[i].m_speed /= 2;
                                mons[i].m_ench[p] = 0;
                                if (mons[i].m_ench[0] == 0 && mons[i].m_ench[1] == 0 && mons[i].m_ench[2] == 0)
                                    mons[i].m_ench_1 = 0;
                            }
                            break;

/*
   case 3: // you[0].paralysis
   //mons [i].m_targ_1_x = mons [i].m_x;
   //mons [i].m_targ_1_y = mons [i].m_y;
   //mons [i].m_beh = 0;
   //mons [i].m_speed += 100;
   if (random2(20) == 0 < mons_HD [i])
   {
   //      cprintf("iggy");
   //      getch();
   //      mons [i].m_speed -= 100;
   //mons [i].m_ev += 5;
   mons [i].m_ench [p] = 0;
   mons [i].m_speed_inc = 0;
   //mons [i].m_speed /= 2;
   //mons [i].m_ench [p] = 0;
   //mons [i].m_beh =
   if (mons [i].m_ench [0] == 0 && mons [i].m_ench [1] == 0 && mons [i].m_ench [2] == 0)
   mons [i].m_ench_1 = 0;
   } else
   {
   mons [i].m_speed_inc = 0; // yuck!
   }
   break;
 */

                        case 4: // fear

                            mons[i].m_beh = 3;
                            if (random2(150) <= mons[i].m_HD + 10)
                            {
                                mons[i].m_beh = 1;      // reset to monster's original behaviour

                                mons[i].m_ench[p] = 0;
                                if (mons[i].m_ench[0] == 0 && mons[i].m_ench[1] == 0 && mons[i].m_ench[2] == 0)
                                    mons[i].m_ench_1 = 0;
                            }
                            break;


                        case 5: // confusion

                            mons[i].m_beh = 4;
                            if (random2(80) < mons[i].m_HD + 10)
                            {
                                if (mons[i].m_class == 66 | mons[i].m_class == 21 | mons[i].m_class == 141)
                                    continue;   // butterfly, fire vortex

                                mons[i].m_beh = 1;
                                mons[i].m_ench[p] = 0;
                                if (mons[i].m_ench[0] == 0 && mons[i].m_ench[1] == 0 && mons[i].m_ench[2] == 0)
                                    mons[i].m_ench_1 = 0;
                            }
                            break;


                        case 6: // invisibility

                            if (random2(40) == 0 | (mons[i].m_class >= MLAVA0 && mons[i].m_sec == 0) | (mons[i].m_class == 125 && random2(3) == 0))
                            {
                                if (mons[i].m_class == 46 | mons[i].m_class == 141)
                                    continue;
                                if (mons[i].m_class >= MLAVA0 && mons[i].m_sec == 1)
                                    continue;   //grid [mons [i].m_x] [mons [i].m_y] == 61) continue;

                                mons[i].m_ench[p] = 0;
                                if (mons[i].m_ench[0] == 0 && mons[i].m_ench[1] == 0 && mons[i].m_ench[2] == 0)
                                    mons[i].m_ench_1 = 0;
                            }
                            break;



                        case 57:        // poison

                        case 58:        // worse poison

                        case 59:        // worser poison

                        case 60:        // worsest poison

                        case 7: // your poison

                        case 8: // your worse poison

                        case 9: // your worser poison

                        case 10:        // your worsest poison

                            if (random2(2) == 0)
                                mons[i].m_hp -= random2((mons[i].m_ench[p] % 50) - 5) + 1;
                            if (mons[i].m_hp <= 0)
                            {
                                if (mons[i].m_ench[p] < 50)
                                    killer = 1;         // ie you

                                else
                                    killer = 5;
                                monster_die(i);
                                goto endbat;
                            }
                            if (random2(12) == 0)       //30 - mons_HD [i]) <= 0) // LOOK AT THIS!!!

                            {
                                mons[i].m_ench[p]--;    //= 0;

                                if (mons[i].m_ench[p] % 50 == 6)
                                {
                                    if (mons[i].m_ench[0] == 0 && mons[i].m_ench[1] == 0 && mons[i].m_ench[2] == 0)
                                    {
                                        mons[i].m_ench_1 = 0;
                                    }
                                }
                            }
                            break;

                            // assumption: mons_res_fire has already been checked
                        case 31:        // sticky flame

                        case 32:        // sticky flame

                        case 33:        // sticky flame

                        case 34:        // sticky flame
                            // other monster's:

                        case 61:        // mons sticky flame

                        case 62:        // mons sticky flame

                        case 63:        // mons sticky flame

                        case 64:        // mons sticky flame

                            mons[i].m_hp -= ((random2(5) + random2(5) + 1) * 10) / mons[i].m_speed;
                            if (mons_res_fire(mons[i].m_class) == -1)
                                mons[i].m_hp -= ((random2(5) + random2(5) + 1) * 10) / mons[i].m_speed;
                            if (mons_near(i) == 1)
                            {
                                strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));
                                strcat(info, " burns!");
                                incrl();
                            }
                            if (mons[i].m_hp <= 0)
                            {
                                if (mons[i].m_ench[p] < 60)
                                    killer = 1;         // ie you

                                else
                                    killer = 5;
                                monster_die(i);
                                goto endbat;
                            }
                            if (random2(2) == 0)
                            {
                                mons[i].m_ench[p]--;    //= 0;

                                if (mons[i].m_ench[p] == 30 | mons[i].m_ench[p] == 60)
                                {
                                    if (mons[i].m_ench[0] == 0 && mons[i].m_ench[1] == 0 && mons[i].m_ench[2] == 0)
                                    {
                                        mons[i].m_ench_1 = 0;
                                    }
                                }
                            }
                            break;

// 19 is taken by summoning:
                            // If these are changed, must also change abjuration
                        case 25:
                        case 24:
                        case 23:
                        case 22:
                        case 21:
                        case 20:
                            if (random2(10) == 0)
                            {
                                mons[i].m_ench[p]--;
                            }
                            if (mons[i].m_ench[p] == 19)
                            {
                                killer = 6;
                                monster_die(i);
                                goto endbat;
                            }
                            break;

                        case 30:        // charm monster

                            if (mons[i].m_beh != 7 | random2(500) <= mons[i].m_HD + 10)
                            {
                                mons[i].m_beh = 1;      // reset to monster's original behaviour

                                mons[i].m_ench[p] = 0;
                                if (mons[i].m_ench[0] == 0 && mons[i].m_ench[1] == 0 && mons[i].m_ench[2] == 0)
                                    mons[i].m_ench_1 = 0;
                            }
                            break;

                        }       // end of switch

                    }           // end of for p












































                    //mons_ench_f(mons_ench, mons_speed, mons_ench_1, mons_beh, i);


















                }               // end of if


                if (mons[i].m_speed >= 100)
                    continue;

                if (mons[i].m_class == 25 | mons[i].m_class == 51 | mons[i].m_class == 107 | mons[i].m_class == 108)
                    mons[i].m_hp_max = mons[i].m_hp;

                if (mons[i].m_hp < mons[i].m_hp_max && (random2(25) == 0 | mons[i].m_class == 8 | mons[i].m_class == 45 | mons[i].m_class == 76 | mons[i].m_class == 135 | mons[i].m_class == 160 | mons[i].m_class == 234 | mons[i].m_class == 221 | mons[i].m_class == 227 | mons[i].m_class == 292))
                    mons[i].m_hp++;



//if (gmon_use [mons [i].m_class] == 1)
                //{
                //if ((mons [i].m_inv [0] != 255 && mons [i].m_inv [1] != 255 && mons [i].m_inv [2] != 255 && mons [i].m_inv [3] != 255) | random2(10) < 5)
                //continue;


//              if (gmon_use [mons [i].m_class] > 3)// && random2(3) == 0) Not used right now -
                // no monster currently has gmon_use > 3. To you[0].speed up, maybe give
                // some advanced monsters ability to use items, but not kobolds etc.

                if (igrid[mons[i].m_x][mons[i].m_y] != 501 && (gmon_use[mons[i].m_class] == 3 | mons[i].m_class == 35))
                {
                    mons_pickup();
                }

//              {
                //              mons_pickup();

/*
   for (o = 0; o < ITEMS; o ++)
   {
   if (itc > it_no) break;


   if (it[0].iquant [o] != 0)
   {
   itc++;

   //if (gmon_use [mons [i].m_class] == 0) break; //continue;

   if (it[0].ix [o] == mons [i].m_x && it[0].iy [o] == mons [i].m_y && it[0].iquant [o] > 0)
   {

   if ((mons [i].m_inv [0] == 501 | it[0].iquant [mons [i].m_inv [0]] == 0) && it[0].iclass [o] == 0 && mons [i].m_inv [0] != o)
   {
   mons [i].m_inv [0] = o; // picking up weapons.
   brek = 1;
   break;
   //strcpy(info, "Hello!"); incrl(); more();
   }
   if ((mons [i].m_inv [1] == 501 | it[0].iquant [mons [i].m_inv [1]] == 0) && it[0].iclass [o] == 1 && mons [i].m_inv [1] != o)
   {
   mons [i].m_inv [1] = o;
   brek = 1;
   break;
   }
   //strcpy(info, "Hello2!"); incrl(); more();
   }

   }
   }
   itc = 0;
 */
//              if (brek == 1)
                //              {
                //              brek = 0;
                //              continue;
                //              }
                //              } // end of if gmon_use


/*
   for (o = 0; o < 200; o ++)
   {
   if (it[0].ix [o] == mons [i].m_x && it[0].iy [o] == mons [i].m_y && it[0].iquant [o] > 0)
   {

   if (mons [i].m_inv [0] == 255 && (it[0].iclass [o] == 0 | it[0].iclass [o] == 1))
   {
   mons [i].m_inv [0] = o; // picking up weapons.
   //strcpy(info, "Hello!"); incrl(); more();
   }
   //strcpy(info, "Hello2!"); incrl(); more();
   }

   }

 */

//} // end of if (gmon_use ...)

// if monster can see you...

// use a switch here....


                if (mons[i].m_beh == 3)         // fleeing

                {
                    if (mons[i].m_targ_1_x < mons[i].m_x)
                        mmov_x = 1;
                    else
                        mmov_x = 0;
                    if (mons[i].m_targ_1_x > mons[i].m_x)
                        mmov_x = -1;
                    if (mons[i].m_targ_1_y < mons[i].m_y)
                        mmov_y = 1;
                    else
                        mmov_y = 0;
                    if (mons[i].m_targ_1_y > mons[i].m_y)
                        mmov_y = -1;
                }
                else
                {
                    if (mons[i].m_targ_1_x < mons[i].m_x)
                        mmov_x = -1;
                    else
                        mmov_x = 0;
                    if (mons[i].m_targ_1_x > mons[i].m_x)
                        mmov_x = 1;
                    if (mons[i].m_targ_1_y < mons[i].m_y)
                        mmov_y = -1;
                    else
                        mmov_y = 0;
                    if (mons[i].m_targ_1_y > mons[i].m_y)
                        mmov_y = 1;
                }

                if (mgrid[mons[i].m_x + mmov_x][mons[i].m_y + mmov_y] != MNG && mgrid[mons[i].m_x + mmov_x][mons[i].m_y + mmov_y] != i)
                {
                    int mnt = mgrid[mons[i].m_x + mmov_x][mons[i].m_y + mmov_y];

                    if (mons[i].m_beh == 7 && mons[mnt].m_beh != 7)
                        mons[i].m_hit = mnt;
                    if (mons[i].m_beh != 7 && mons[mnt].m_beh == 7)
                        mons[i].m_hit = mnt;
                }


                brkk = 0;

                if (mons[i].m_beh == 4)         // confused

                {
                    mmov_x = random2(3) - 1;
                    mmov_y = random2(3) - 1;
                    if (mgrid[mons[i].m_x + mmov_x][mons[i].m_y + mmov_y] != MNG && (mmov_x != 0 | mmov_y != 0))
                    {
//              for (p = 0; p < MNST; p ++)
                        //              {
                        //              if (mons_x [p] == mons [i].m_x + mmov_x && mons_y [p] == mons [i].m_y + mmov_y && mons_class [p] > -1)
                        //              {
                        if (monsters_fight(i, mgrid[mons[i].m_x + mmov_x][mons[i].m_y + mmov_y]) == 1)
                        {
                            mmov_x = 0;
                            mmov_y = 0;
                            brkk = 1;
                            break;
                        }
//              }
                        //              }
                    }
                }               // end of if confused

                if (brkk == 1)
                    continue;


//if ((mons [i].m_class != 1 && mons [i].m_class != 46) | bat == 0)
                //{

// MONS FIGHT WAS HERE!!!!

// for monsters who use spec abils even when next to you
                if (mons_near(i) == 1)
                {

                    switch (mons[i].m_class)
                    {

                    case 59:
                        if (random2(2) == 0 && (mons[i].m_ench[2] != 6 | you[0].see_invis != 0))
                        {
                            strcpy(info, "The giant eyeball stares at you!");
                            incrl();
                            if (you[0].paralysis < 10)
                                you[0].paralysis += 2 + random2(3);
                        }
                        break;

                    case 65:
                        if (random2(2) == 0 && (mons[i].m_ench[2] != 6 | you[0].see_invis != 0))
                        {
                            strcpy(info, "The eye of draining stares at you!");
                            incrl();
                            you[0].ep -= random2(5) + random2(5) + random2(5) + 2;
                            if (you[0].ep < 0)
                                you[0].ep = 1;
                            ep_ch = 1;
                            mons[i].m_hp += 5;
                            if (mons[i].m_hp >= mons[i].m_hp_max)
                                mons[i].m_hp = mons[i].m_hp_max;
                        }
                        break;

                    case MLAVA0:
                    case MLAVA1:
                    case MLAVA2:
                    case MLAVA3:
                    case MLAVA4:
                    case MWATER0:
                    case MWATER1:
                    case MWATER2:
                    case MWATER3:
                    case MWATER4:
                    case MWATER5:
                        if (mons[i].m_sec == 0)         // visible

                        {
                            if (random2(5) == 0)
                            {
                                mons[i].m_sec = 1;
                                mons[i].m_ench[2] = 6;
                                mons[i].m_ench_1 = 1;

                            }
                        }
                        else
                        {
                            if (random2(10) == 0 && (mons[i].m_x != you[0].x_pos | mons[i].m_y != you[0].y_pos))
                                mons[i].m_sec = 0;
                        }
                        break;

                    case 125:   // air elemental

                        if (random2(5) == 0)
                        {
                            mons[i].m_ench[2] = 6;
                            mons[i].m_ench_1 = 1;
                        }
                        break;

                    }           // end of switch

                }               // end of if mons_near



// ATTACK WAS HERE!!!








                if (((mons[i].m_beh == 1 | mons[i].m_beh == 3) && mons_near(i) == 1) | ((mons[i].m_beh == 6 | mons[i].m_beh == 7) && mons[i].m_hit != MHITNOT));
                {

                    if ((mons[i].m_beh == 1 | mons[i].m_beh == 3) && mons_near(i) == 1)
                        you[0].pet_target = i;

                    if ((mons[i].m_beh == 6 | mons[i].m_beh == 7) && (mons[i].m_hit == MHITNOT | mons[i].m_hit == MHITYOU))
                    {
                        goto end_switch;
// break;
                    }

                    if (mons[i].m_beh == 6 | mons[i].m_beh == 7)
                    {
                        trac_targ = mons[i].m_hit;
                        trac_targ_x = mons[mons[i].m_hit].m_x;
                        trac_targ_y = mons[mons[i].m_hit].m_y;
                    }
                    else
                    {
                        trac_targ = MHITYOU;
                        trac_targ_x = you[0].x_pos;
                        trac_targ_y = you[0].y_pos;
                    }

                    if (trac_targ == MHITYOU && mons_near(i) == 0 && mons[i].m_class != 234)
                        goto end_switch;        //continue;

// Hmmm... monsters will only fight with missiles/you[0].spells if they can see you!
                    //    big problem!

// Could solve: make mons_near(i) == 1 only necess if attacking you;
                    // if not attacking you, rely on tracer. This will be VERY slow...

                    switch (mons[i].m_class)
                    {

                    case MLAVA2:
                        if (you[0].invis != 0 && mons_see_invis(mons[i].m_class) == 0)
                            break;
                        if (random2(2) == 0 | mons[i].m_sec == 1 | mons[i].m_ench[2] == 6)
                            break;
// viewwindow was here.
                        if (show[mons[i].m_x - you[0].x_pos + 6][mons[i].m_y - you[0].y_pos + 6] != 0)
                        {
                            move_x = 0;
                            move_y = 0;
//move_x = you[0].x_pos - mons [i].m_x;
                            //move_y = you[0].y_pos - mons [i].m_y;

                            move_x = trac_targ_x - mons[i].m_x;
                            move_y = trac_targ_y - mons[i].m_y;
                            if (move_x != 0 | move_y != 0)
                            {
                                brek = 1;
                                viewwindow();
                                target_x = trac_targ_x;
                                target_y = trac_targ_y;
                                aim_down = 1;
                                beam_range = random2(10) + 5;
                                beam_source_x = mons[i].m_x;
                                beam_source_y = mons[i].m_y;
//              strcpy(beam_name, monam (mons [i].m_sec, mons [i].m_class, mons [i].m_ench [2], 4)); //gmon_name [mons_class [i]]);
                                strcpy(beam_name, "glob of lava");
                                beam_damage = 108;
                                beam_colour = RED;
                                beam_type = '#';        // hash #

                                beam_flavour = 20;      // lava

                                beam_hit = 20;
                                strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));      //gmon_name [mons_class [i]]);

                                strcat(info, " spits lava!");
                                incrl();
                                thing_thrown = 4;
                                missile();
                            }
                        }
                        break;

                    case MWATER2:
                        if (you[0].invis != 0 && mons_see_invis(mons[i].m_class) == 0)
                            break;
                        if (mons[i].m_sec == 1 | mons[i].m_ench[2] == 6)
                            break;
                        if (show[mons[i].m_x - you[0].x_pos + 6][mons[i].m_y - you[0].y_pos + 6] != 0)
                        {
                            move_x = 0;
                            move_y = 0;
                            move_x = trac_targ_x - mons[i].m_x;
                            move_y = trac_targ_y - mons[i].m_y;
                            if (move_x != 0 | move_y != 0)
                            {
                                target_x = trac_targ_x;
                                target_y = trac_targ_y;
                                aim_down = 1;
                                brek = 1;
                                viewwindow();
                                beam_range = random2(10) + 5;
                                beam_source_x = mons[i].m_x;
                                beam_source_y = mons[i].m_y;
//              strcpy(beam_name, monam (mons [i].m_sec, mons [i].m_class, mons [i].m_ench [2], 4)); //gmon_name [mons_class [i]]);
                                strcpy(beam_name, "bolt of electricity");
                                beam_damage = 105;
                                beam_colour = LIGHTCYAN;
                                beam_type = 35;         // hash #

                                beam_flavour = 5;       // elec

                                beam_hit = 150;
                                strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));      //gmon_name [mons_class [i]]);

                                strcat(info, " shoots out a bolt of electricity!");
                                incrl();
                                thing_thrown = 2;
                                beam();
                            }
                        }
                        break;

                    case 61:
                        if (you[0].invis != 0 && mons_see_invis(mons[i].m_class) == 0)
                            break;
                        if (random2(3) == 0)    // o plant

                        {
                            plant_spit();
                        }
                        break;

                    case 31:
                        if (random2(4) == 0)    // fiend!

                        {
                            switch (random2(4))
                            {
                            case 0:
                                spell_cast = 48;
                                mons_cast();
                                break;
                            case 1:
                            case 2:
                            case 3:
                                if (trac_targ == MHITYOU && mons_near(i) == 0)
                                    break;
                                tracer_f();
                                if (mons[i].m_beh == 7 && tracer == 1)
                                    break;
                                if (tracer != 0 | (trac_targ != MHITYOU && trac_hit_mons != 0))         // doesn't need to worry about you[0].haste

                                {
                                    if (mons[i].m_ench[2] != 6)
                                    {
                                        strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));
                                        strcat(info, " makes a gesture!");
                                        incrl();
                                    }
                                    spell_cast = 49;
                                    mons_cast();
                                }
                                break;

                            }

                            mmov_x = 0;
                            mmov_y = 0;

                        }
                        break;


// Kobold demonologist
                    case 53:
                        if (random2(15) == 0 && no_mons < 90)
                        {
                            //summon_demon();
                            mmov_x = 0;
                            continue;
                        }
                        break;

                    case 15:
                    case 140:
                        if (random2(7) == 0)    // phantom

                        {
                            if (mons_near(i) == 1)
                            {
                                strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));
                                strcat(info, " blinks.");
                                incrl();
                            }
                            monster_blink(i);
                        }
                        break;



//case 68:
                        //place_cloud(101, mons [i].m_x, mons [i].m_y, 2 + random2(4));
                        //break;






                    case 12:
                        if (you[0].invis != 0 && mons_see_invis(mons[i].m_class) == 0)
                            break;
                        if (mons[i].m_inv[1] != 501)    // manticore

                        {
                            hand_used = mons[i].m_inv[1];
                            beam_range = 6;
                            //              strcpy(beam_name, fake_name [hand_used]);//it[0].itype [mons [i].m_inv [1]]]);
                            item_name(it[0].iplus2[hand_used], it[0].iclass[hand_used], it[0].itype[hand_used], it[0].idam[hand_used], it[0].iplus[hand_used], 1, it[0].iid[hand_used], 6, str_pass);
                            strcpy(beam_name, str_pass);        //"dart");

                            mons_throw();
                            //mmov_x = 0;
                            if (brek == 1)
                            {
                                brek = 0;
                                continue;
                            }
                        }
                        break;


// dragon breath weapon:
                    case 29:
                    case 73:    // hellhound

                    case 75:
                    case 306:   // Patrick

                        if (you[0].invis != 0 && mons_see_invis(mons[i].m_class) == 0)
                            break;
                        if ((mons[i].m_class != 73 && random2(13) < 3) | random2(10) == 0)
                        {
                            if (mons[i].m_beh == 6 | mons[i].m_beh == 7)
                            {
                                trac_targ = mons[i].m_hit;
                                tracer_f();
                                if (trac_hit_mons == 0)
                                    break;
                            }
                            dragon();
                            mmov_x = 0;
//mmov_y = 0;
                            continue;
//break;
                        }       // end of dragon breathe

                        break;

                    }           // end switch??

// SPELLCASTER MOVED HERE
                    if (mons_flag(mons[i].m_class, M_SPELLCASTER))
                    {
                        spell_cast = 100;
                        if (mons[i].m_class == 80)
                            switch (random2(3))
                            {
                            case 0:
                                mons[i].m_sec = RED;
                                break;
                            case 1:
                                mons[i].m_sec = LIGHTRED;
                                break;
                            case 2:
                                mons[i].m_sec = YELLOW;
                                break;
                            }

                        if (you[0].invis != 0 && mons_see_invis(mons[i].m_class) == 0)
                            break;

                        if (random2(200) > 50 + mons[i].m_HD)
                            break;

                        msecc = mons[i].m_sec;
                        if (mons[i].m_class == 80)
                            msecc = 30;         // burning devil

//aim_down = 1;

/*splist [0] = 8; // bolt spell
   splist [1] = 6; // enchantment
   splist [2] = 11; // self_ench
   splist [3] = 0; // misc */

// tracer = 0 = run out of range
                        // tracer = 1 = hits you in range
                        // tracer_mons = 0 = hits no monsters
                        //       "     = 1 = hits monster before you (possibly also after)
                        //       "     = 2 = hits monster after but not before
                        //       "     = 3 = hits tamed monster(s) but no others

                        int func_pass_2[6];

                        mons_spell_list(msecc, func_pass_2);

//if (mons_near(i) == 0 && func_pass_2 [4] == 18) goto digging;
                        if (func_pass_2[4] == 18 && mons_near(i) == 0 && mons[i].m_beh == 1)    // && trac_targ == MHITYOU)// && distance(you[0].x_pos, mons [i].m_x, you[0].y_pos, mons [i].m_y) < 40)

                        {
                            spell_cast = 18;    // This is EVIL!
                            //        strcpy(info, "Digging!");
                            //        incrl();

                            goto casted;
                        }

                        if (trac_targ == MHITYOU && mons_near(i) == 0)
                            break;

                        tracer_f();

/*
   for (okg = 0; okg < 6; okg ++)
   {
   itoa(func_pass_2 [okg], st_prn, 10);
   strcpy(info, st_prn);
   incrl();
   }
   * /
   digging : if (func_pass_2 [4] == 18 && mons_near(i) == 0 && mons [i].m_beh == 1 && trac_targ == MHITYOU && distance(you[0].x_pos, mons [i].m_x, you[0].y_pos, mons [i].m_y) < 40)
   {
   spell_cast = 18;
   goto casted;
   }
 */
                        if (func_pass_2[4] == 18)
                            func_pass_2[4] = 100;

//   if (tracer != 0 | (trac_targ != MHITYOU && trac_hit_mons != 0))

                        if (mons[i].m_beh == 7 && trac_hit_tamed == 1)
                            break;

                        if (mons[i].m_beh != 3)
                        {
                            //outp(tracer_mons);
                            if (tracer_mons == 0 && tracer == 1 | ((trac_targ != MHITYOU && trac_hit_mons != 0)))
                            {
                                spell_cast = func_pass_2[random2(5)];
                                goto casted;
                            }
                            if (tracer_mons == 1 && trac_targ == MHITYOU | random2(10) == 0)    // | (tracer == 0 && trac_targ == MHITYOU))

                            {
                                if (random2(2) == 0 | (mons[i].m_ench_1 == 1))
                                    break;
                                spell_cast = func_pass_2[2];
                                goto casted;
                            }
                            //if (tracer == 1 && tracer_mons == 3) spell_cast = mons_spell_list(msecc, 0);
                            if ((tracer == 1 && (tracer_mons != 1)) | (trac_targ != MHITYOU))   // && trac_hit_mons == 1)) //tracer_mons == 2 | tracer_mons == 3))

                            {
                                switch (random2(4))
                                {
                                case 0:
                                    spell_cast = func_pass_2[0];
                                    if (tracer_mons == 2)
                                        aim_down = 1;
                                    break;
                                case 1:
                                    spell_cast = func_pass_2[1];
                                    break;
                                case 2:
                                    spell_cast = func_pass_2[3];
                                    break;
                                case 3:
                                    spell_cast = func_pass_2[4];
                                    break;
                                }
                            }
                        }       // end if

                        else
                        {
                            if (random2(3) == 0)
                                break;  // if (random2(5) != 0)

                            spell_cast = func_pass_2[5];
                        }

                      casted:if (spell_cast == 100)
                            break;      // nothing cast

                        if (mons_near(i) == 1)
                        {
                            strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));  //gmon_name [mons_class [i]]);

                            if (mons[i].m_class != 69)  // brain worm

                                if (spell_cast == 33 | spell_cast == 34)
                                {
                                    strcat(info, " opens a gate.");
                                }
                                else if (spell_cast == 27 && you[0].your_level < 50)
                                {
                                    if (mons[i].m_ench[2] != 6)
                                    {
                                        if (you[0].your_level < 50)
                                            strcat(info, " opens a gate to Hell!");
                                        else
                                            strcat(info, " calls for assistance.");
                                        incrl();
                                    }
                                }
                                else if (mons[i].m_class == 101 | mons[i].m_class == 122 | mons[i].m_class == 132 | mons[i].m_class == 146)     // steam, mottled, storm and golden dragons

                                {
                                    if (mons[i].m_ench[2] != 6)
                                    {
                                        strcat(info, " breathes.");
                                        incrl();
                                    }
                                }
                                else if (mons[i].m_class == 79)         // great orb of eyes

                                {
                                    if (mons[i].m_ench[2] != 6)
                                    {
                                        strcat(info, " gazes.");
                                        incrl();
                                    }
                                }
                                else if (mons[i].m_class == 141)        // vapour

                                {
                                    mons[i].m_ench[2] = 6;
                                }
                                else if (mons[i].m_class == 70)         // giant orange brain

                                {
                                    if (mons[i].m_ench[2] != 6)
                                    {
                                        strcat(info, " pulsates.");
                                        incrl();
                                    }
                                }
                                else if (mons[i].m_class == 340)
                                {
                                    strcat(info, " winds a great silver horn.");
                                    incrl();
                                }
                                else if ((mons[i].m_class >= 80 && mons[i].m_class <= 90) | mons[i].m_class == 68 | (mons[i].m_class >= 220 && mons[i].m_class <= 234))
                                {
                                    strcat(info, " gestures.");
                                    incrl();
                                }
                                else
                                {
                                    strcat(info, " casts a spell.");
                                    incrl();
                                }
                        }       // end of if mons_near

                        else if (mons[i].m_class == 340)
                        {
                            strcpy(info, "You hear a weird and mournful sound.");
                            incrl();    // Geryon

                        }


                        if (spell_cast == 16)
                        {
                            if (mons_near(i) == 1)
                            {
//              strcpy(info, monam (mons [i].m_sec, mons [i].m_class, mons [i].m_ench [2], 0)); //gmon_name [mons [i].m_class]);
                                //              strcat(info, " casts a spell!");
                                //              incrl();
                                strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));      //gmon_name [mons_class [i]]);

                                strcat(info, " blinks!");
                                incrl();
                                monster_blink(i);
                                continue;
                            }
                        }

/* switch(random2(3))
   {
   case 0: spell_cast = 0; break;
   case 1: spell_cast = 4; break;
   case 2: spell_cast = 5; break;
   case 3: spell_cast = 1; break;
   }
 */

// spell_cast = random2(13);

                        //if (spell_cast == 105) spell_cast = 104;

//      if (spell_cast != 5 && spell_cast != 11)
                        //      {
                        //              tracer_f();
                        //      }

                        beam_damage = 5 * mons[i].m_HD;         // really???

//      if (spell_cast == 5 | spell_cast == 11 | spell_cast == 13 | spell_cast == 14) // doesn't need to worry about you[0].haste
                        //      {
                        mons_cast();
//      }

//      it[0].iquant [mons [i].m_inv [4]] --;
                        //      if (it[0].iquant [mons [i].m_inv [4]] == 0) mons [i].m_inv [4] = 501; // = it[0].ilink [mons [i].m_inv [4]] ???
                        mmov_x = 0;
                        trac_hit_tamed = 0;
                        continue;
//}
                    }

                  end_switch:if (gmon_use[mons[i].m_class] > 0 && mons[i].m_inv[1] != 501 && mons[i].m_class != 30 && (you[0].invis == 0 | mons_see_invis(mons[i].m_class) != 0))
                                                                                                                                                                                // 2-h ogre

                    {

                        hand_used = mons[i].m_inv[1];
                        if (random2(10) < 8)
                        {
                            if (it[0].iquant[hand_used] > 0)
                            {
/*      brek = 0;
   beam_colour = 200;
   beam_name [0] = 48;
   beam_type = 0;
   beam_range = 6; // should be same as range is going to be for real missile.
   move_x = trac_targ_x - mons [i].m_x;
   move_y = trac_targ_y - mons [i].m_y;
   beam_source_x = mons [i].m_x;
   beam_source_y = mons [i].m_y;
   beam(); */
                                if (trac_targ == MHITYOU && tracer_mons == 1)
                                    goto end_throw;
                                if (trac_targ == MHITYOU && mons_near(i) == 0)
                                    goto end_throw;
                                tracer_f();
                                if (mons[i].m_beh == 7 && tracer == 1 && tracer_mons != 1)
                                    goto end_throw;

                                if (tracer != 0 | (trac_targ != MHITYOU && trac_hit_mons != 0))         // doesn't need to worry about you[0].haste

                                {
                                    beam_range = 6;
//              strcpy(beam_name, fake_name [hand_used]);//it[0].itype [mons [i].m_inv [1]]]);
                                    item_name(it[0].iplus2[hand_used], it[0].iclass[hand_used], it[0].itype[hand_used], it[0].idam[hand_used], it[0].iplus[hand_used], 1, it[0].iid[hand_used], 6, str_pass);
                                    strcpy(beam_name, str_pass);
                                    mons_throw();
                                    //mmov_x = 0;
                                    if (brek == 1)
                                    {
                                        brek = 0;
                                        continue;
                                    }
                                }

//      if (tracer != 0)
                                //      {
                                //break;
                                //      } // end of if brek
                            }
                        }

                    }

                }               // end if mons_near



              end_throw:if ((mons[i].m_beh == 7 | mons[i].m_beh == 6) && (mmov_x != 0 | mmov_y != 0) && mgrid[mons[i].m_x + mmov_x][mons[i].m_y + mmov_y] != MNG)
                {
//              for (p = 0; p < MNST; p ++)
                    //              {
                    //              if (mons_x [p] == mons [i].m_x + mmov_x && mons_y [p] == mons [i].m_y + mmov_y && mons_class [p] > -1)
                    //              {
                    if (monsters_fight(i, mgrid[mons[i].m_x + mmov_x][mons[i].m_y + mmov_y]) == 1)
                    {
                        if (mons[i].m_class == 1 | mons[i].m_class == 46)
                            mons[i].m_speed_inc -= mons[i].m_speed;
                        mmov_x = 0;
                        mmov_y = 0;
                        brkk = 1;
                        break;
                    }
//              }
                    //      }
                }               // end of if mons [i].m_beh == 7
                //} // end of bat

                if (brkk == 1)
                    continue;





                if (mons[i].m_x + mmov_x == you[0].x_pos && mons[i].m_y + mmov_y == you[0].y_pos)
                {
//o = 4;//mons [i].m_class;
                    //j = 5;//it[0].iclass [mons [i].m_inv [0]];
                    //k = 6;//it[0].itype [mons [i].m_inv [0]];
                    //p = 7;//property [j] [k] [0];
                    //p = 8;
                    //mmov_y = 1;
                    //mmov_x = property [it[0].iclass [mmov_y]] [it[0].itype [mmov_y]] [0];
                    //mmov_x = property [it[0].iclass [mons [i].m_inv [0]]] [it[0].itype [mons [i].m_inv [0]]] [0];


                    mmov_x = mons[i].m_inv[0];

                    if (mons[i].m_class == 1 | mons[i].m_class == 46 && bat == 0)
                        monster_attack(i);

                    if ((mons[i].m_class == 1 | mons[i].m_class == 46) && mons[i].m_beh != 7)   // giant bat

                    {
                        mons[i].m_beh = 2;
                        bat = 1;
                    }



                    if (mons[i].m_class != 1 && mons[i].m_class != 46)
                        monster_attack(i);


                    if (mons[i].m_class == 32 && mons[i].m_hp < 1)
                    {
                        for (j = 0; j < 3; j++)
                        {
                            mons[i].m_ench[j] = 0;
                        }

                        mons[i].m_ench_1 = 0;
                        //itc = 0;
                        no_mons--;
                        mons[i].m_class = -1;
//      if (grid [mons [i].m_x] [mons [i].m_y] >= 50) grid [mons [i].m_x] [mons [i].m_y] -= 50;
                        mgrid[mons[i].m_x][mons[i].m_y] = MNG;
                        spore_goes_pop();       // has to be after the above, so that spore isn't killed twice.

                        continue;
                    }           // end of if giant spore



                    mmov_x = 0;
                    mmov_y = 0;
                    continue;   //break;

                }



                if (mons[i].m_class == -1 | mons[i].m_class == 61)
                    continue;
                // o plant
                monster_move();



/*
   if (mons [i].m_x > you[0].x_pos - 6 && mons [i].m_x < you[0].x_pos + 6 && mons [i].m_y > you[0].y_pos - 6 && mons [i].m_y < you[0].y_pos + 6 && show [mons [i].m_x - you[0].x_pos + 6] [mons [i].m_y - you[0].y_pos + 6] != 0)
   {
   brek = 1;
   viewwindow();
   }
 */

                if (mons[i].m_beh == 1)         //: // chasing YOU

                    if (mons_near(i) == 1 && (you[0].invis == 0 | mons_see_invis(mons[i].m_class) != 0))        // && random2 (6) != 0))

                    {
                        mons[i].m_targ_1_x = you[0].x_pos;
                        mons[i].m_targ_1_y = you[0].y_pos;
                    }

                if (mons[i].m_hp <= (mons[i].m_hp_max / 4 - 1) && mons_intel(mons[i].m_class) > 1)
                    mons[i].m_beh = 3;



            }
          endbat:bat = 0;

            if (mons[i].m_beh == 6 && random2(3) != 0)
                mons[i].m_beh = 1;      // I think?

        }                       // end of if (mons_class != -1)

    }
//}
    //}

//if (mons_near(i) == 1)
    {
//itoa(monc, st_prn, 10);
        //mons [i].m_speed, st_prn, 10);
        //strcpy(info, st_prn);
        //strcat(info, ",");
        //itoa(mons [i].m_speed_inc, st_prn, 10);
        //strcat(info, st_prn);
        //incrl();
    }



}                               // end of void monster()


void tracer_f(void)
{

    trac_hit_mons = 0;
    tracer = 0;
    tracer_mons = 0;
    brek = 0;
    beam_colour = 200;
    beam_name[0] = 48;
    beam_type = 0;
    beam_range = 10;
    move_x = trac_targ_x - mons[i].m_x;
    move_y = trac_targ_y - mons[i].m_y;
    beam_source_x = mons[i].m_x;
    beam_source_y = mons[i].m_y;
    aim_down = 0;
    beam();
//  trac_targ_x = 0;
    //              trac_targ_y = 0;
    //  trac_targ = MHITNOT;
}





void mons_pickup(void)
{

    if (mons[i].m_class == 35)  // Jelly!

    {
//      if (random2(3) == 0) return;

        // need to make sure it doesn't eat artefacts etc.

        int hps_gained;
        unsigned int quant_eated;

        quant_eated = random2(10) + 1;

        if (quant_eated > it[0].iquant[igrid[mons[i].m_x][mons[i].m_y]])
            quant_eated = it[0].iquant[igrid[mons[i].m_x][mons[i].m_y]];

        if (it[0].iclass[igrid[mons[i].m_x][mons[i].m_y]] == 0 && it[0].idam[igrid[mons[i].m_x][mons[i].m_y]] > 180)
            return;             // unique items

        if (it[0].iclass[igrid[mons[i].m_x][mons[i].m_y]] == 1 && (it[0].itype[igrid[mons[i].m_x][mons[i].m_y]] == 0 | it[0].itype[igrid[mons[i].m_x][mons[i].m_y]] == 5))
        {
            // shouldn't eat stone things.
            // - but what about stone wands & rings?
            return;
        }

        if (it[0].iclass[igrid[mons[i].m_x][mons[i].m_y]] < 15)
        {
            hps_gained = quant_eated * mass[it[0].iclass[igrid[mons[i].m_x][mons[i].m_y]]][it[0].itype[igrid[mons[i].m_x][mons[i].m_y]]];
            hps_gained /= 20;
//        hps_gained = random2(10) + 3;
        }
        else
        {
            hps_gained = 1;
        }

        if (hps_gained < 1)
            hps_gained = 1;
        if (hps_gained > 50)
            hps_gained = 50;


        it[0].iquant[igrid[mons[i].m_x][mons[i].m_y]] -= quant_eated;
        if (it[0].iquant[igrid[mons[i].m_x][mons[i].m_y]] <= 0)
            igrid[mons[i].m_x][mons[i].m_y] = it[0].ilink[igrid[mons[i].m_x][mons[i].m_y]];


        mons[i].m_hp += hps_gained;
        if (mons[i].m_hp > mons[i].m_hp_max)
            mons[i].m_hp_max = mons[i].m_hp;
        // adjust for mass of object!!!

        if (mons_near(i) == 1)
        {
            strcpy(info, "You hear a slurping noise.");
            incrl();
        }
        else
        {
            strcpy(info, "You hear a distant slurping noise.");
            incrl();
        }


        if (mons[i].m_hp >= 50)
        {

            if (jelly_divide(i) == 0)
                mons[i].m_hp = 50;      //abort();

            //jelly_divide(i);

            /*no_mons ++;
               mn_no ++;

               mons_beh [k] = 0; // Look at this! */

        }

        return;

    }




    switch (it[0].iclass[igrid[mons[i].m_x][mons[i].m_y]])
    {
    case 0:
        if (mons[i].m_inv[0] != 501)
            return;
        if (it[0].idam[igrid[mons[i].m_x][mons[i].m_y]] > 180)
            return;             // too tricky for now

        if ((mons_charclass(mons[i].m_class) == 36 | mons_charclass(mons[i].m_class) == 6) && property[it[0].iclass[igrid[mons[i].m_x][mons[i].m_y]]][it[0].itype[igrid[mons[i].m_x][mons[i].m_y]]][1] <= 0)
            return;             // wimpy monsters shouldn't pick up halberds etc

        mons[i].m_inv[0] = igrid[mons[i].m_x][mons[i].m_y];
        igrid[mons[i].m_x][mons[i].m_y] = it[0].ilink[igrid[mons[i].m_x][mons[i].m_y]];
        it[0].ilink[mons[i].m_inv[0]] = 501;
        if (it[0].idam[mons[i].m_inv[0]] % 30 == 7)
            mons[i].m_AC += 3;
        strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));
        strcat(info, " picks up ");
        it_name(mons[i].m_inv[0], 3);
        strcat(info, str_pass);
        strcat(info, ".");
        if (mons_near(i))
            incrl();
        mmov_x = 0;
        mmov_y = 0;
        break;

    case 1:
        if (mons[i].m_inv[1] != 501 && it[0].itype[mons[i].m_inv[1]] == it[0].itype[igrid[mons[i].m_x][mons[i].m_y]] && it[0].iplus[mons[i].m_inv[1]] == it[0].iplus[igrid[mons[i].m_x][mons[i].m_y]] && it[0].iplus2[mons[i].m_inv[1]] == it[0].iplus2[igrid[mons[i].m_x][mons[i].m_y]] && it[0].idam[mons[i].m_inv[1]] == it[0].idam[igrid[mons[i].m_x][mons[i].m_y]])
        {
            it[0].iquant[mons[i].m_inv[1]] += it[0].iquant[igrid[mons[i].m_x][mons[i].m_y]];
            it[0].iquant[igrid[mons[i].m_x][mons[i].m_y]] = 0;
            igrid[mons[i].m_x][mons[i].m_y] = it[0].ilink[igrid[mons[i].m_x][mons[i].m_y]];
            it[0].ilink[mons[i].m_inv[0]] = 501;
            return;
        }
        if (mons[i].m_inv[0] != 501 | it[0].iquant[igrid[mons[i].m_x][mons[i].m_y]] == 1)
            return;
        mons[i].m_inv[1] = igrid[mons[i].m_x][mons[i].m_y];
        igrid[mons[i].m_x][mons[i].m_y] = it[0].ilink[igrid[mons[i].m_x][mons[i].m_y]];
        it[0].ilink[mons[i].m_inv[0]] = 501;
        strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));
        strcat(info, " picks up ");
        it_name(mons[i].m_inv[1], 3);
        strcat(info, str_pass);
        strcat(info, ".");
        if (mons_near(i))
            incrl();
        mmov_x = 0;
        mmov_y = 0;
        break;

    case 14:
        if (mons[i].m_class != 13)
            break;
        mons[i].m_hp += random2(mons_weight(it[0].iplus[igrid[mons[i].m_x][mons[i].m_y]])) / 100 + 1;
        if (mons[i].m_hp > 77)
            mons[i].m_hp = 77;
        if (mons[i].m_hp > mons[i].m_hp_max)
            mons[i].m_hp_max = mons[i].m_hp;
        strcpy(info, monam(mons[i].m_sec, mons[i].m_class, mons[i].m_ench[2], 0));
        strcat(info, " eats ");
        it_name(mons[i].m_inv[0], 1);
        strcat(info, str_pass);
        strcat(info, ".");
        if (mons_near(i))
            incrl();
        destroy_item(igrid[mons[i].m_x][mons[i].m_y]);
        break;

    }


/*
   for (o = 0; o < ITEMS; o ++)
   {
   if (itc > it_no) break;


   if (it[0].iquant [o] != 0)
   {
   itc++;

   //if (gmon_use [mons_class [i]] == 0) break; //continue;

   if (it[0].ix [o] == mons [i].m_x && it[0].iy [o] == mons [i].m_y && it[0].iquant [o] > 0)
   {

   if ((mons [i].m_inv [0] == 501 | it[0].iquant [mons [i].m_inv [0]] == 0) && it[0].iclass [o] == 0 && mons [i].m_inv [0] != o)
   {
   mons [i].m_inv [0] = o; // picking up weapons.
   brek = 1;
   break;
   //strcpy(info, "Hello!"); incrl(); more();
   }
   if ((mons [i].m_inv [1] == 501 | it[0].iquant [mons [i].m_inv [1]] == 0) && it[0].iclass [o] == 1 && mons [i].m_inv [1] != o)
   {
   mons [i].m_inv [1] = o;
   brek = 1;
   break;
   }
   //strcpy(info, "Hello2!"); incrl(); more();
   }

   }
   }
   itc = 0;
 */
}
#endif
