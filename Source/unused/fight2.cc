char monsters_fight(int monster_attacking, int monster_attacked)
{
    int damage_taken = 0;
    char hit = 0;
    int mmov_x = 0;
    char str_pass[80];
    char water_attack = 0;
    int specdam = 0;

    int hand_used = 0;

    int sees = 0;

    if (menv[monster_attacking].m_class == MONS_GIANT_SPORE)
    {
        menv[monster_attacking].m_hp = -1;
        return 0;
    }

    if (menv[monster_attacking].m_class >= MONS_LAVA_WORM && menv[monster_attacking].m_sec == 1 && menv[monster_attacked].m_class < MONS_LAVA_WORM)
        return 0;

    if (menv[monster_attacking].m_beh == BEH_ENSLAVED)
    {
        if (menv[monster_attacked].m_beh == BEH_ENSLAVED)
            return 0;
        menv[monster_attacking].m_hit = monster_attacked;
    }

    if (menv[monster_attacked].m_beh == BEH_FIGHT)
    {
        if (menv[monster_attacked].m_beh != BEH_ENSLAVED && monster_attacked != menv[monster_attacking].m_hit)
            return 0;
    }

    if (grd[menv[monster_attacking].m_x][menv[monster_attacking].m_y] == DNGN_SHALLOW_WATER && mons_flies(menv[monster_attacking].m_class) == 0 && menv[monster_attacking].m_class < MONS_LAVA_WORM)
    {
        if ( one_chance_in(4) )
        {
            mpr("You hear a splashing noise.");
            return 1;
        }
    }

    if (grd[menv[monster_attacked].m_x][menv[monster_attacked].m_y] == DNGN_SHALLOW_WATER && mons_flies(menv[monster_attacked].m_class) == 0 && menv[monster_attacking].m_class >= MONS_BIG_FISH)
        water_attack = 1;

    if (mons_near(monster_attacking) && mons_near(monster_attacked))
        sees = 1;

    char runthru;

    for (runthru = 0; runthru < 4; runthru++)
    {

        char mdam = mondamage(menv[monster_attacking].m_class, runthru);

        if (menv[monster_attacking].m_class == MONS_SMALL_ZOMBIE || menv[monster_attacking].m_class == MONS_BIG_ZOMBIE || menv[monster_attacking].m_class == MONS_SMALL_SKELETON || menv[monster_attacking].m_class == MONS_LARGE_SKELETON || menv[monster_attacking].m_class == MONS_SPECTRAL_THING)
        {
            mdam = mondamage(menv[monster_attacking].m_sec, runthru);
            if (mdam > 1 && mdam < 4)
            {
                mdam--;
            }

            if (mdam > 3 && mdam < 8)
            {
                mdam -= 2;
            }

            if (mdam > 7 && mdam < 13)
            {
                mdam -= 3;
            }

            if (mdam > 12)
            {
                mdam -= 5;
            }
        }


        if (mdam == 0)
            break;

        if (menv[monster_attacking].m_class == MONS_TWO_HEADED_OGRE && runthru == 1 && menv[monster_attacking].m_inv[1] != ING)
        {
            hand_used = 1;
        }

        damage_taken = 0;


        int mons_to_hit = 20 + menv[monster_attacking].m_HD;    // * menv [monster_attacking].m_HD; // * 3  //you[0].strength + (0.5 * you[0].you[0].max_dex) + (you[0].f_abil);

        mons_to_hit += 5 * water_attack;

        if (menv[monster_attacking].m_inv[hand_used] != ING)
        {
            if (mitm.iplus[menv[monster_attacking].m_inv[hand_used]] - 50 > 130)
            {
                mons_to_hit += mitm.iplus[menv[monster_attacking].m_inv[hand_used]] - 50 - 100;         //random2(mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 - 100 + 1);

            }
            else
            {
                mons_to_hit += mitm.iplus[menv[monster_attacking].m_inv[hand_used]] - 50;       //random2(mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 + 1); /// 10;

            }

            mons_to_hit += 3 * property(OBJ_WEAPONS, mitm.itype[menv[monster_attacking].m_inv[0]], PWPN_HIT);

            menv[monster_attacking].m_speed_inc -= ((property(you[0].inv_class[you[0].equip[EQ_WEAPON]], you[0].inv_type[you[0].equip[EQ_WEAPON]], PWPN_SPEED) - 10) / 2);      // / 10);

        }

        mons_to_hit = random2(mons_to_hit);


        if (menv[monster_attacking].m_class == MONS_KILLER_BEE || menv[monster_attacking].m_class == MONS_WORM)
        {
            menv[monster_attacking].m_speed_inc -= 10;  //--

        }






        if (mons_to_hit >= menv[monster_attacked].m_ev || ((menv[monster_attacked].m_speed_inc <= 60 || menv[monster_attacked].m_beh == BEH_SLEEP) && !one_chance_in(20) ))
        {
            hit = 1;


            if (menv[monster_attacking].m_inv[hand_used] != ING && mitm.iclass[menv[monster_attacking].m_inv[hand_used]] == OBJ_WEAPONS && (mitm.itype[menv[monster_attacking].m_inv[hand_used]] < WPN_SLING || mitm.itype[menv[monster_attacking].m_inv[hand_used]] > WPN_HAND_CROSSBOW))
            {

                damage_taken = random2(property(mitm.iclass[menv[monster_attacking].m_inv[0]], mitm.itype[menv[monster_attacking].m_inv[0]], PWPN_DAMAGE));

                if (mitm.idam[menv[monster_attacking].m_inv[hand_used]] / 30 == DWPN_ORCISH && mons_charclass(menv[monster_attacking].m_class) == MONS_ORC)
                {
                    damage_taken += random2(2);
                }

//if (mitm.iplus [mons_inv [i] [0]] > 80) damage_taken -= 100;
                //damage_taken += mitm.iplus [mons_inv [i] [0]];
                if (mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] > 130)
                {
                    if (mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] - 150 >= 0)       /* + or 0 to-dam */
                        damage_taken += random2(mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] - 150 + 1);
                    else
                        damage_taken -= random2(abs(mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] - 150 + 1));  /* - to-dam */
                }
                else
                {
                    if (mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] - 50 >= 0)        /* + or 0 to-dam */
                        damage_taken += random2(mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] - 50 + 1);
                    else
                        damage_taken -= random2(abs(mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] - 50 + 1));   /* - to-dam */
                }
/*        if (mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] > 130)
   {
   damage_taken += random2(mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] - 150 + 1);
   } else
   {
   damage_taken += random2(mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] - 50 + 1);// % 10;
   } */
                damage_taken -= random2(3) + 1;         //1;

            }

            damage_taken += random2(mdam) + 1;

            damage_taken *= water_attack + 1;

//gmon_att [menv [monster_attacking].m_class] [runthru]) + 1;
            //damage_taken /= (random2 (player_AC()) + 1); // / 3
            //damage_taken *= 2;

//if (menv [monster_attacking].m_inv [0] != ING)
            //{
            //      damage_taken /= ((random2 (player_AC()) / property [mitm.iclass [mons_inv [i] [0]]] [mitm.itype [mons_inv [i] [0]]] [2]) + 1);
            //} else
            //      damage_taken /= (random2 (player_AC()) + 1);

            damage_taken -= random2(menv[monster_attacked].m_AC + 1);

            if (damage_taken < 1)
                damage_taken = 0;
//you[0].hp -= (int) damage_taken;
            //ouch(damage_taken, monster_attacking, 0);

//you[0].hp_ch = 1;

        }

        else
        {
            hit = 0;
//      strcpy(info, "The ");
            //} else
            //strcat(info, "The ");

            int mmov_x = menv[monster_attacking].m_inv[hand_used];

            char str_pass[80];

            if (menv[monster_attacking].m_class == MONS_DANCING_WEAPON)
            {
                item_name(mitm.iplus2[mmov_x], mitm.iclass[mmov_x], mitm.itype[mmov_x], mitm.idam[mmov_x], mitm.iplus[mmov_x], mitm.iquant[mmov_x], mitm.iid[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));      //gmon_name [menv [monster_attacking].m_class]);

            strcat(info, " misses ");
            strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
            strcat(info, ".");
            if (sees == 1)
                mpr(info);
        }


        if (damage_taken < 1 && hit == 1)
        {
//if (78 - strlen(info) < strlen(gmon_name [menv [monster_attacking].m_class]) + 17)
            //      {
            //mpr(info);
            //      strcpy(info, "The ");
            //} else
            //strcat(info, "The ");


            int mmov_x = menv[monster_attacking].m_inv[hand_used];
            char str_pass[80];

            if (menv[monster_attacking].m_class == MONS_DANCING_WEAPON)
            {
                item_name(mitm.iplus2[mmov_x], mitm.iclass[mmov_x], mitm.itype[mmov_x], mitm.idam[mmov_x], mitm.iplus[mmov_x], mitm.iquant[mmov_x], mitm.iid[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));      //gmon_name [menv [monster_attacking].m_class]);

            strcat(info, " hits ");
            strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
#ifdef WIZARD
            itoa(damage_taken, st_prn, 10);
            strcat(info, " for ");
            strcat(info, st_prn);       // note: doesn't take account of special weapons etc
#endif
            strcat(info, ".");  // but doesn't do any you[0].damage.");

            if (sees == 1)
                mpr(info);
        }



        if (hit == 1)           //(int) damage_taken >= 1)

        {
//if (78 - strlen(info) < strlen(gmon_name [menv [monster_attacking].m_class]) + 15)
            //      {
            //      mpr(info);
            //      strcpy(info, "The ");
            //      } else
            //      strcpy(info, "The ");

            int mmov_x = menv[monster_attacking].m_inv[hand_used];

            if (menv[monster_attacking].m_class == MONS_DANCING_WEAPON)
            {
                char str_pass[80];

                item_name(mitm.iplus2[mmov_x], mitm.iclass[mmov_x], mitm.itype[mmov_x], mitm.idam[mmov_x], mitm.iplus[mmov_x], mitm.iquant[mmov_x], mitm.iid[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));
            strcat(info, " hits ");
            strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));


/*        strcat(info, " for ");
   itoa(damage_taken, st_prn, 10);
   strcat(info, st_prn);
 */


//      itoa(

            if (menv[monster_attacking].m_class != MONS_DANCING_WEAPON && menv[monster_attacking].m_inv[hand_used] != ING && mitm.iclass[menv[monster_attacking].m_inv[hand_used]] == OBJ_WEAPONS && (mitm.itype[menv[monster_attacking].m_inv[hand_used]] < WPN_SLING || mitm.itype[menv[monster_attacking].m_inv[hand_used]] > WPN_HAND_CROSSBOW))
            {
                strcat(info, " with ");

                item_name(mitm.iplus2[mmov_x], mitm.iclass[mmov_x], mitm.itype[mmov_x], mitm.idam[mmov_x], mitm.iplus[mmov_x], mitm.iquant[mmov_x], mitm.iid[mmov_x], 3, str_pass);     // was 7

                strcat(info, str_pass);
            }
            strcat(info, "! "); // put something for '.', '!', '!!' etc depending on what % of you[0].damage you took.

            if (sees == 1)
                mpr(info);

            // special attacks:

            /* brek = 0; */



            switch (menv[monster_attacking].m_class)
            {
            case MONS_GIANT_ANT:
            case MONS_WOLF_SPIDER:
            case MONS_REDBACK:
            case MONS_SPINY_WORM:
            case MONS_JELLYFISH:
            case MONS_ORANGE_DEMON:     // demon

                if ((damage_taken >= 4 && one_chance_in(4) ) || one_chance_in(20) || menv[monster_attacking].m_class == MONS_SPINY_WORM)
                {
                    strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));
                    strcat(info, " stings ");
                    strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
                    strcat(info, ".");
                    if (sees == 1)
                        mpr(info);
                    //        you[0].poison++;
                    poison_monster(monster_attacked, 1);
                }
                break;

            case MONS_CENTAUR:  // cockatrice.

                break;

            case MONS_KILLER_BEE:       // killer bee

            case MONS_BUMBLEBEE:
                if (((damage_taken >= 3 && one_chance_in(3) ) || one_chance_in(20) ))
                {
                    strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));
                    strcat(info, " stings ");
                    strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
                    strcat(info, ".");
                    if (sees == 1)
                        mpr(info);
                    // you[0].poison += 1;
                    poison_monster(monster_attacked, 1);
                }
                break;

            case MONS_NECROPHAGE:       // necrophage

            case MONS_ROTTING_DEVIL:    /* rotting devil */
            case MONS_GHOUL:    /* ghoul */
            case MONS_DEATH_OOZE:       // death ooze

                if (mons_holiness(menv[monster_attacked].m_class) >= MH_UNDEAD)
                    break;
                if ((damage_taken >= 3 && one_chance_in(3) ) || one_chance_in(20) )
                {
                    //strcpy(info, "You feel your flesh start to rot away!");
                    //mpr(info);
                    //you[0].poison += random2(3) + 1;
                    menv[monster_attacked].m_hp_max -= random2(3) + 1;
                    if (menv[monster_attacked].m_hp > menv[monster_attacked].m_hp_max)
                        menv[monster_attacked].m_hp = menv[monster_attacked].m_hp_max;
                }
                break;

            case MONS_FIRE_VORTEX:      // fire_vortex

                menv[monster_attacking].m_hp = -10;
            case MONS_FIRE_ELEMENTAL:   // fire elem

            case MONS_BALRUG:   // balrug

            case MONS_SUN_DEMON:        // sun demon

                specdam = 0;
                if (mons_res_fire(menv[monster_attacked].m_class) == 0)
                {
                    specdam = random2(15) + 15;
                    if (menv[monster_attacked].m_inv[2] != ING && mitm.idam[menv[monster_attacked].m_inv[2]] % 30 == 2)
                        specdam /= 3;
                }
                if (mons_res_fire(menv[monster_attacked].m_class) == -1 || (menv[monster_attacked].m_inv[2] == ING || mitm.idam[menv[monster_attacked].m_inv[2]] % 30 != 2))
                {
                    specdam = random2(25) + 20;
                }
                if (specdam != 0)
                {
                    if (sees == 1)
                    {
                        strcpy(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 0));
                        strcat(info, " is engulfed in flame!");
                        //if (sees == 1)
                        mpr(info);
                    }
                }
                damage_taken += specdam;
                break;

            case MONS_QUEEN_BEE:        // queen bee

            case MONS_GIANT_CENTIPEDE:          // giant centipede

            case MONS_SOLDIER_ANT:      // soldier ant

            case MONS_QUEEN_ANT:        /* Queen ant */
//      if ((damage_taken >= 3 && one_chance_in(3) ) || one_chance_in(20) )
                //      {
                strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));
                strcat(info, " stings ");
                strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
                strcat(info, ".");
                if (sees == 1)
                    mpr(info);
                poison_monster(monster_attacked, 1);
//      you[0].poison += 2;
                //      }
                break;


            case MONS_SCORPION: // snake

            case MONS_BROWN_SNAKE:      // br snake

            case MONS_BLACK_SNAKE:      // black snake

            case MONS_YELLOW_SNAKE:     // yellow snake

            case MONS_GOLD_MIMIC:
            case MONS_WEAPON_MIMIC:
            case MONS_ARMOUR_MIMIC:
            case MONS_SCROLL_MIMIC:
            case MONS_POTION_MIMIC:     /* Mimics */
                if ((damage_taken >= 3 && one_chance_in(4) ) || one_chance_in(20) )
                {
                    strcpy(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 0));
                    strcat(info, " is poisoned.");
                    if (sees == 1)
                        mpr(info);
                    poison_monster(monster_attacked, 1);
                }
                break;

            case MONS_SHADOW_DRAGON:    // shadow dragon

            case MONS_SPECTRAL_THING:   /* spectral thing */
                if ( coinflip() )
                    break;
            case MONS_WIGHT:    // wight

            case MONS_WRAITH:   // wraith

            case MONS_SOUL_EATER:       // soul eater

            case MONS_SHADOW_FIEND:     // sh F

            case MONS_SPECTRAL_WARRIOR: // spectre

            case MONS_ORANGE_RAT:       // orange rat

            case MONS_ANCIENT_LICH:     // ancient lich

                if (mons_holiness(menv[monster_attacked].m_class) >= MH_UNDEAD)
                    break;
                if ( ( damage_taken >= 6 && coinflip() ) || one_chance_in(30) )
                {
                    strcpy(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 0));
                    strcat(info, " is drained.");
                    if (sees == 1)
                        mpr(info);
                    if ( one_chance_in(5) )
                        menv[monster_attacked].m_HD--;
                    menv[monster_attacked].m_hp_max -= 2 + random2(3);
                    menv[monster_attacked].m_hp -= 2 + random2(3);
                    if (menv[monster_attacked].m_hp >= menv[monster_attacked].m_hp_max)
                        menv[monster_attacked].m_hp = menv[monster_attacked].m_hp_max;
                    if (menv[monster_attacked].m_hp <= 0 || menv[monster_attacked].m_HD <= 0)
                    {
                        monster_die(monster_attacked, KILL_MON, monster_attacking);
                        return 1;
                    }
                    //brek = 1;
                }
                break;

            case MONS_WORM:     // giant wasp
                /*      if ((damage_taken >= 3 && one_chance_in(3) ) || one_chance_in(20) )
                   {
                   if (you[0].paralysis > 0)
                   {
                   strcpy(info, "You still can't move!");
                   } else strcpy(info, "You suddenly lose the ability to move!");
                   mpr(info);
                   you[0].paralysis += random2(3) + 1;
                   } */
                break;

            case MONS_JELLY:    // Jelly
                //      strcpy(info, "You are splashed with acid!");
                //      mpr(info);
                //      splash_with_acid(10);

                break;




            case MONS_ICE_DEVIL:        // ice devil

            case MONS_ICE_BEAST:        // Ice beast

            case MONS_FREEZING_WRAITH:          // blue wraith

            case MONS_ICE_FIEND:        // I Fiend

            case MONS_WHITE_IMP:        // lesser demon

            case MONS_AZURE_JELLY:      // azure jelly

            case MONS_ANTAEUS:  // Antaeus

                specdam = 0;
                if (mons_res_cold(menv[monster_attacked].m_class) == 0)
                {
                    specdam = random2(menv[monster_attacking].m_HD * 2) + menv[monster_attacking].m_HD;
                    if (menv[monster_attacked].m_inv[2] != ING && mitm.idam[menv[monster_attacked].m_inv[2]] % 30 == 3)
                        specdam = (random2(menv[monster_attacking].m_HD * 2) + menv[monster_attacking].m_HD) / 3;
                }
                if (mons_res_cold(menv[monster_attacked].m_class) == -1 && (menv[monster_attacked].m_inv[2] == ING || mitm.idam[menv[monster_attacked].m_inv[2]] % 30 != 3))
                {
                    specdam = random2(menv[monster_attacking].m_HD * 3) + menv[monster_attacking].m_HD * 2;
                }
                if (specdam != 0)
                {
                    if (sees == 1)
                    {
                        strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));
                        strcat(info, " freezes ");
                        strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
                        strcat(info, ".");
                        if (sees == 1)
                            mpr(info);
                    }
                }
                damage_taken += specdam;
                break;

            case MONS_VAMPIRE:  // Vampire
                /* if ( damage_taken >= 7 && coinflip() || one_chance_in(10) )
                   menv [monster_attacked].m_hp_max -= 3;
                   if (menv [monster_attacked].m_hp >= menv [monster_attacked].m_hp_max) menv [monster_attacked].m_hp = menv [monster_attacked].m_hp_max;
                   menv [monster_attacking].m_hp += random2(8);
                   if (menv [monster_attacking].m_hp > menv [monster_attacking].m_hp_max) menv [monster_attacking].m_hp = menv [monster_attacking].m_hp_max; */
                if (mons_holiness(menv[monster_attacked].m_class) >= MH_UNDEAD)
                    break;
                strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));
                strcat(info, " is healed.");
                if (mons_near(monster_attacking))
                    mpr(info);
                menv[monster_attacking].m_hp += random2(damage_taken);
                if (menv[monster_attacking].m_hp > menv[monster_attacking].m_hp_max)
                    menv[monster_attacking].m_hp = menv[monster_attacking].m_hp_max;
                break;




            case MONS_GUARDIAN_NAGA:    // Naga

                break;


            }                   // end of switch for special attacks.


        }
/* special weapons */
        if (hit == 1 && (menv[monster_attacking].m_inv[hand_used] != ING || ((menv[monster_attacking].m_class == MONS_PLAYER_GHOST || menv[monster_attacking].m_class == MONS_PANDEMONIUM_DEMON) && ghost.ghs[8] != 0)))
        {
            unsigned char itdam;

            if (menv[monster_attacking].m_class == MONS_PLAYER_GHOST || menv[monster_attacking].m_class == MONS_PANDEMONIUM_DEMON)
            {
                itdam = ghost.ghs[8];
            }
            else
            {
                itdam = mitm.idam[menv[monster_attacking].m_inv[hand_used]];
            }

            specdam = 0;

            if (itdam == 188)
                goto flaming;   // sword of Okawaru

            if (menv[monster_attacking].m_class == MONS_PLAYER_GHOST || menv[monster_attacking].m_class == MONS_PANDEMONIUM_DEMON || mitm.idam[menv[monster_attacking].m_inv[hand_used]] < 180)
                switch (itdam % 30)
                {

                case SPWPN_NORMAL:      // nothing

                    break;

                case SPWPN_FLAMING:     // flaming

                  flaming:specdam = 0;
                    if (mons_res_fire(menv[monster_attacked].m_class) == 0)
                    {
                        specdam = random2(damage_taken) / 2 + 1;
                        if (menv[monster_attacked].m_inv[2] != ING && mitm.idam[menv[monster_attacked].m_inv[2]] % 30 == 2)
                            specdam /= 3;
                    }
                    if (mons_res_fire(menv[monster_attacked].m_class) == -1 && (menv[monster_attacked].m_inv[2] == ING || mitm.idam[menv[monster_attacked].m_inv[2]] % 30 != 2))
                    {
                        specdam = random2(damage_taken) + 1;
                    }
                    if (specdam != 0)
                    {
                        if (sees == 1)
                        {
                            int mmov_x = menv[monster_attacking].m_inv[hand_used];
                            char str_pass[80];

                            if (menv[monster_attacking].m_class == MONS_DANCING_WEAPON)
                            {
                                item_name(mitm.iplus2[mmov_x], mitm.iclass[mmov_x], mitm.itype[mmov_x], mitm.idam[mmov_x], mitm.iplus[mmov_x], mitm.iquant[mmov_x], mitm.iid[mmov_x], 0, str_pass);
                                strcpy(info, str_pass);
                            }
                            else
                                strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));
                            strcat(info, " burns ");
                            strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
                            if (specdam < 3)
                                strcat(info, ".");
                            if (specdam >= 3 && specdam < 7)
                                strcat(info, "!");
                            if (specdam >= 7)
                                strcat(info, "!!");
                            //    strcat(info, " is burned.");
                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_FREEZING:    // freezing

                    specdam = 0;
                    if (mons_res_cold(menv[monster_attacked].m_class) == 0)
                    {
                        specdam = random2(damage_taken) / 2 + 1;
                        if (menv[monster_attacked].m_inv[2] != ING && mitm.idam[menv[monster_attacked].m_inv[2]] % 30 == 3)
                            specdam = (random2(damage_taken) / 2 + 1) / 3;
                    }
                    if (mons_res_cold(menv[monster_attacked].m_class) == -1 && (menv[monster_attacked].m_inv[2] == ING || mitm.idam[menv[monster_attacked].m_inv[2]] % 30 != 3))
                    {
                        specdam = random2(damage_taken) + 1;
                    }
                    if (specdam != 0)
                    {
                        if (sees == 1)
                        {
                            mmov_x = menv[monster_attacking].m_inv[hand_used];

                            if (menv[monster_attacking].m_class == MONS_DANCING_WEAPON)
                            {
                                item_name(mitm.iplus2[mmov_x], mitm.iclass[mmov_x], mitm.itype[mmov_x], mitm.idam[mmov_x], mitm.iplus[mmov_x], mitm.iquant[mmov_x], mitm.iid[mmov_x], 0, str_pass);
                                strcpy(info, str_pass);
                            }
                            else
                                strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));
                            strcat(info, " freezes ");
                            strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
                            if (specdam < 3)
                                strcat(info, ".");
                            if (specdam >= 3 && specdam < 7)
                                strcat(info, "!");
                            if (specdam >= 7)
                                strcat(info, "!!");
                            //    strcat(info, " is burned.");
                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_HOLY_WRATH:          // holy wrath

                    if (menv[monster_attacking].m_class == MONS_PLAYER_GHOST)
                        break;
                    specdam = 0;
                    switch (mons_holiness(menv[monster_attacked].m_class))
                    {
                    case -1:
//    strcpy(info, "
                        damage_taken -= 5 + random2(5);
                        break;

                    case 1:
                        specdam += random2(damage_taken) + 1;
                        break;

                    case 2:
                        specdam += (random2(damage_taken) * 15) / 10 + 1;       // does * 1.5 do anything?

                        break;
                    }
                    break;

                case SPWPN_ELECTROCUTION:       // electrocution
                    //   if (you[0].lev != 0) break; // you're not grounded
                    //   if (menv [monster_attacking].m_class == 400) break;

                    if (mons_flies(menv[monster_attacked].m_class) == 2)
                        break;
                    if (mons_res_elec(menv[monster_attacked].m_class) != 0)
                        break;
                    specdam = 0;

                    if (mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] <= 50 || mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] > 130 && mitm.iplus2[menv[monster_attacking].m_inv[hand_used]] <= 150)
                        break;
                    if ( one_chance_in(3) )
                    {
                        strcpy(info, "There is a sudden explosion of sparks!");
                        if (sees == 1)
                            mpr(info);
                        specdam += 10 + random2(15);
//    mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] --;
                    }
                    break;

                case SPWPN_ORC_SLAYING: // orc slaying

                    if (mons_charclass(menv[monster_attacked].m_class) == MONS_ORC)
//   if (menv [monster_attacked].m_class == 14 || menv [monster_attacked].m_class == 52 || menv [monster_attacked].m_class == 54 || menv [monster_attacked].m_class == 55 || menv [monster_attacked].m_class == 103)
                    {
                        menv[monster_attacked].m_hp -= 1 + random2(damage_taken);
                    }
                    break;

                case SPWPN_VENOM:       // venom

                    if ( one_chance_in(3) )
                        poison_monster(monster_attacked, 0);
                    break;

//   case 7: // protection
                case SPWPN_DRAINING:
                    if (mons_holiness(menv[monster_attacked].m_class) > MH_NORMAL && ((damage_taken >= 6 && coinflip() ) || one_chance_in(30) ))
                    {
                        strcpy(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 0));
                        strcat(info, " is drained.");
                        if (sees == 1)
                            mpr(info);
                        if ( one_chance_in(5) )
                            menv[monster_attacked].m_HD--;
                        menv[monster_attacked].m_hp_max -= 2 + random2(3);
                        menv[monster_attacked].m_hp -= 2 + random2(3);
                        if (menv[monster_attacked].m_hp >= menv[monster_attacked].m_hp_max)
                            menv[monster_attacked].m_hp = menv[monster_attacked].m_hp_max;
                        if (menv[monster_attacked].m_hp <= 0 || menv[monster_attacked].m_HD <= 0)
                        {
                            monster_die(monster_attacked, KILL_MON, monster_attacking);
                            return 1;
                        }
                        specdam = random2(damage_taken) / 2 + 1;
                    }
                    break;

                case SPWPN_SPEED:       // you[0].speed

                    menv[monster_attacking].m_speed_inc -= menv[monster_attacking].m_speed / 2;
                    break;

                case SPWPN_VORPAL:      // slicing etc

                    specdam += (random2(damage_taken) / 2) + 1;
                    break;

                case SPWPN_VAMPIRICISM: // vampiric

                    specdam = 0;        // note does no extra damage

                    if (mons_holiness(menv[monster_attacked].m_class) > MH_NORMAL)
                        break;
                    if ( one_chance_in(5) || menv[monster_attacking].m_hp == menv[monster_attacking].m_hp_max)
                        break;
                    menv[monster_attacking].m_hp += random2(damage_taken) + 1;  // thus is probably more valuable on larger weapons?

                    if (menv[monster_attacking].m_hp > menv[monster_attacking].m_hp_max)
                        menv[monster_attacking].m_hp = menv[monster_attacking].m_hp_max;
//   if (you[0].hunger <= 11000) you[0].hunger += random2(30);
                    if (mons_near(monster_attacking) && (menv[monster_attacking].m_ench[2] != 6 || player_see_invis() != 0))
                    {
                        if (menv[monster_attacking].m_class == MONS_DANCING_WEAPON)
                        {
                            item_name(mitm.iplus2[mmov_x], mitm.iclass[mmov_x], mitm.itype[mmov_x], mitm.idam[mmov_x], mitm.iplus[mmov_x], mitm.iquant[mmov_x], mitm.iid[mmov_x], 0, str_pass);
                            strcpy(info, str_pass);
                        }
                        else
                            strcpy(info, monam(menv[monster_attacking].m_sec, menv[monster_attacking].m_class, menv[monster_attacking].m_ench[2], 0));
                        strcat(info, " is healed.");
                        mpr(info);
                    }
                    break;

                case SPWPN_DISRUPTION:          // mace of disruption

                    if (menv[monster_attacking].m_class == MONS_PLAYER_GHOST)
                        break;
                    specdam = 0;
                    if (mons_holiness(menv[monster_attacked].m_class) == MH_UNDEAD && !one_chance_in(3) )
                    {
                        strcpy(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 0));
                        strcat(info, " shudders.");
                        if (mons_near(monster_attacked))
                            mpr(info);
                        specdam += random2(damage_taken + 1);
                        specdam += random2(damage_taken + 1);
                        specdam += random2(damage_taken + 1);
/*    if (menv [monster_attacked].m_ench [1] >= 20 && menv [monster_attacked].m_ench [1] <= 25)
   menv [monster_attacked].m_ench [1] -= 1;
   if (menv [monster_attacked].m_ench [1] == 19) menv [monster_attacked].m_ench [1] == 20; */
                    }
                    break;


                case SPWPN_DISTORTION:          // distortion
                    //   if ( !one_chance_in(3) ) break;

                    if ( one_chance_in(3) )
                    {
                        strcpy(info, "Space bends around ");
                        strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
                        strcat(info, ".");
                        if (mons_near(monster_attacked))
                            mpr(info);
                        specdam += random2(5) + random2(3) + 1;
                        break;
                    }
                    if ( one_chance_in(3) )
                    {
                        strcpy(info, "Space warps horribly around ");
                        strcat(info, monam(menv[monster_attacked].m_sec, menv[monster_attacked].m_class, menv[monster_attacked].m_ench[2], 1));
                        strcat(info, "!");
                        if (mons_near(monster_attacked))
                            mpr(info);
                        specdam += random2(12) + random2(13) + 3;
                        break;
                    }
                    if ( one_chance_in(3))
                    {
                        monster_blink(monster_attacked);
                        break;
                    }
                    if ( coinflip() )
                    {
                        monster_teleport(monster_attacked, 0);
                        break;
                    }
                    if ( coinflip() )
                    {
                        monster_teleport(monster_attacked, 1);
                        break;
                    }
                    if ( coinflip() )
                    {
                        monster_die(monster_attacked, KILL_RESET, monster_attacking);
                        break;
                    }
                    break;



                }
        }                       // end of if special weapon

        damage_taken += specdam;

        if (damage_taken > 0)
        {
            //ouch(damage_taken, monster_attacking, 0);
            //you[0].hp_ch = 1;
            menv[monster_attacked].m_hp -= damage_taken;
            if (menv[monster_attacked].m_hp <= 0)
            {
                monster_die(monster_attacked, KILL_MON, monster_attacking);
                return 1;
            }
        }


    }                           // end of for runthru

    if (menv[monster_attacked].m_beh == BEH_ENSLAVED)
    {
//   menv [monster_attacked].m_beh = 6;
        menv[monster_attacked].m_hit = monster_attacking;
        menv[monster_attacking].m_hit = monster_attacked;
//  }
    }

    if (menv[monster_attacking].m_beh != BEH_CONFUSED)  // && menv [monster_attacking].m_beh != 7) // the latter is so that mons_beh [attacker] doesn't get reset to 6

    {
        if (menv[monster_attacked].m_beh <= BEH_WANDER && distance(you[0].x_pos, menv[monster_attacked].m_x, you[0].y_pos, menv[monster_attacked].m_y) > 1)
        {
            menv[monster_attacked].m_beh = BEH_FIGHT;
            menv[monster_attacked].m_hit = monster_attacking;   // This'll do for now.

        }
    }

    return 1;
}                               // end of void monsters_fight(int monster_attacking, ...)



void monster_die(int monster_killed, char killer, int i)
{
    int dmi;                    /* dead monster's inventory */

    if (you[0].prev_targ == monster_killed)
        you[0].prev_targ = MHITNOT;

    if (menv[monster_killed].m_class == MONS_GIANT_SPORE)
    {
        if (menv[monster_killed].m_hp < 1 && menv[monster_killed].m_hp > -15)
        {
            return;
        }
    }

    if (menv[monster_killed].m_class == MONS_FIRE_VORTEX || menv[monster_killed].m_class == MONS_SPATIAL_VORTEX)
    {
        strcpy(info, monam(menv[monster_killed].m_sec, menv[monster_killed].m_class, menv[monster_killed].m_ench[2], 0));
        strcat(info, " dissipates!");   // <- spelling?

        if (mons_near(monster_killed))
            mpr(info);
        if (killer == 1 || killer == 3)
        {
            gain_exp(exper_value(menv[monster_killed].m_class, menv[monster_killed].m_HD, menv[monster_killed].m_hp_max));
        }
        if (menv[monster_killed].m_class == MONS_FIRE_VORTEX)
            place_cloud(CLOUD_FIRE_MON, menv[monster_killed].m_x, menv[monster_killed].m_y, 2 + random2(4));
        goto out_of_switch;
    }                           /* end fire vortex */

    if (menv[monster_killed].m_class == MONS_DANCING_WEAPON)
    {
        strcpy(info, monam(menv[monster_killed].m_sec, menv[monster_killed].m_class, menv[monster_killed].m_ench[2], 0));
        strcat(info, " falls from the air.");
        if (mons_near(monster_killed))
            mpr(info);
        if (killer == 1 || killer == 3)
        {
            gain_exp(exper_value(menv[monster_killed].m_class, menv[monster_killed].m_HD, menv[monster_killed].m_hp_max));
        }
        goto out_of_switch;
    }                           /* end dancing weapon */

    switch (killer)
    {
    case 1:                     /* You kill in combat. */
    case 3:                     /* You kill by missile or beam. */
        strcpy(info, "You kill ");
        if (wounded_damaged(menv[monster_killed].m_class) == 1)
            strcpy(info, "You destroy ");
        strcat(info, monam(menv[monster_killed].m_sec, menv[monster_killed].m_class, menv[monster_killed].m_ench[2], 1));       //gmon_name [menv [monster_killed].m_class]);

        strcat(info, "!");
        mpr(info);
        gain_exp(exper_value(menv[monster_killed].m_class, menv[monster_killed].m_HD, menv[monster_killed].m_hp_max));
        if (you[0].religion == GOD_XOM && random2(70) <= 10 + menv[monster_killed].m_HD)
            Xom_acts(1, random2(menv[monster_killed].m_HD) + 1, 0);
        if (you[0].duration[DUR_PRAYER] > 0)
        {
            if (mons_holiness(menv[monster_killed].m_class) == MH_NORMAL)
                done_good( GOOD_KILLED_LIVING, menv[monster_killed].m_HD );
            if (mons_holiness(menv[monster_killed].m_class) == MH_UNDEAD)
                done_good( GOOD_KILLED_UNDEAD, menv[monster_killed].m_HD );
            if (mons_holiness(menv[monster_killed].m_class) == MH_DEMONIC)
                done_good( GOOD_KILLED_DEMON, menv[monster_killed].m_HD );
            if (mons_holiness(menv[monster_killed].m_class) == -1)
                done_good( GOOD_KILLED_ANGEL_II, menv[monster_killed].m_HD );
        }
        else if (mons_holiness(menv[monster_killed].m_class) == -1)
            done_good( GOOD_KILLED_ANGEL_I, menv[monster_killed].m_HD );

        if ((you[0].religion == GOD_MAKHLEB && you[0].duration[DUR_PRAYER] != 0 && random2(you[0].piety) >= 30) || you[0].mutation[MUT_DEATH_STRENGTH] != 0)
        {                       /* Makhleb */
            if (you[0].hp < you[0].hp_max)
            {
                mpr("You feel a little better.");
                you[0].hp += menv[monster_killed].m_HD + random2(menv[monster_killed].m_HD);
                if (you[0].hp > you[0].hp_max)
                    you[0].hp = you[0].hp_max;
                you[0].hp_ch = 1;
            }
        }
        if ((you[0].religion == GOD_MAKHLEB || you[0].religion == GOD_VEHUMET) && you[0].duration[DUR_PRAYER] != 0 && random2(you[0].piety) >= 30)
        {                       /* Makhleb or Vehumet */
            if (you[0].ep < you[0].ep_max)
            {
                mpr("You feel your power returning.");
                you[0].ep += random2(random2(menv[monster_killed].m_HD)) + 1;
                if (you[0].ep > you[0].ep_max)
                    you[0].ep = you[0].ep_max;
                you[0].ep_ch = 1;
            }
        }

        if (you[0].duration[DUR_DEATH_CHANNEL] != 0 && mons_holiness(menv[monster_killed].m_class) == MH_NORMAL && mons_weight(mons_charclass(menv[monster_killed].m_class)) != 0)
        {
            if (create_monster(MONS_SPECTRAL_THING, 0, BEH_ENSLAVED, menv[monster_killed].m_x, menv[monster_killed].m_y, you[0].pet_target, mons_charclass(menv[monster_killed].m_class)) != -1)
                mpr("A strange glowing mist starts to gather...");
        }                       /* Death Channel - creates a spectral version of the monster */

        break;

    case 2:                     /* Monster kills in combat */
    case 4:                     /* Monster kills by missile or beam */
        if (mons_near(monster_killed))
        {
            strcpy(info, monam(menv[monster_killed].m_sec, menv[monster_killed].m_class, menv[monster_killed].m_ench[2], 0));   //gmon_name [menv [monster_killed].m_class]);

            strcat(info, " dies!");
            mpr(info);
        }

        if (menv[monster_killed].m_beh == BEH_ENSLAVED)
            naughty(6, (menv[monster_killed].m_HD / 2) + 1);

        if ((i >= 0 && i < 200) && menv[i].m_beh == BEH_ENSLAVED)
        {
            gain_exp(exper_value(menv[monster_killed].m_class, menv[monster_killed].m_HD, menv[monster_killed].m_hp_max) / 2 + 1);
            if (mons_holiness(menv[i].m_class) == MH_UNDEAD)
            {
                if (mons_holiness(menv[monster_killed].m_class) == MH_NORMAL)
                    done_good( GOOD_SLAVES_KILL_LIVING, menv[monster_killed].m_HD );
                else
                    done_good( GOOD_SERVANTS_KILL, menv[monster_killed].m_HD );
            }
            else
            {
                done_good( GOOD_SERVANTS_KILL, menv[monster_killed].m_HD );
                if (you[0].religion == GOD_VEHUMET && random2(you[0].piety) >= 20)
                {               /* Vehumet - only for non-undead servants (coding convenience, no real reason except that Vehumet prefers demons) */
                    if (you[0].ep < you[0].ep_max)
                    {
                        mpr("You feel your power returning.");
                        you[0].ep += random2(random2(menv[monster_killed].m_HD)) + 1;
                        if (you[0].ep > you[0].ep_max)
                            you[0].ep = you[0].ep_max;
                        you[0].ep_ch = 1;
                    }
                }
            }
        }
        break;

    case 5:                     /* Monster killed by trap/inanimate thing/itself/poison not from you */
        if (mons_near(monster_killed))
        {
            strcpy(info, monam(menv[monster_killed].m_sec, menv[monster_killed].m_class, menv[monster_killed].m_ench[2], 0));   //gmon_name [mons_class [i]]);

            strcat(info, " dies!");
            mpr(info);
        }
        break;

    case 6:                     /* Monster doesn't die, just goes back to wherever it came from
                                   This must only be called by monsters running out of time (or abjuration),
                                   because it uses the beam variables! Or does it??? */
        if (mons_near(monster_killed))
        {
            strcpy(info, monam(menv[monster_killed].m_sec, menv[monster_killed].m_class, menv[monster_killed].m_ench[2], 0));   //gmon_name [mons_class [i]]);

            strcat(info, " disappears in a puff of smoke!");
            mpr(info);
            place_cloud(CLOUD_GREY_SMOKE_MON + random2(3), menv[monster_killed].m_x, menv[monster_killed].m_y, 1 + random2(3));
        }
        for (dmi = 7; dmi >= 0; dmi--)  /* takes whatever it's carrying back home */
        {
            if (menv[monster_killed].m_inv[dmi] != ING)
            {
                destroy_item(menv[monster_killed].m_inv[dmi]);
            }
            menv[monster_killed].m_inv[dmi] = ING;
        }
        break;

    }                           /* end of switch */


out_of_switch:
    if (menv[monster_killed].m_class == MONS_MUMMY)
    {
        if (killer == 1 || killer == 3)
        {
            if (curse_an_item(1, 0) == 1)
            {
                strcpy(info, "You feel nervous for a moment...");
                mpr(info);
            }
        }
    }

    if (menv[monster_killed].m_class == MONS_GUARDIAN_MUMMY || menv[monster_killed].m_class == MONS_GREATER_MUMMY || menv[monster_killed].m_class == MONS_MUMMY_PRIEST)         /* other mummies */
    {
        if (killer == 1 || killer == 3)
        {
            mpr("You feel extremely nervous for a moment...");
            miscast_effect(16, 3 + (menv[monster_killed].m_class == MONS_GREATER_MUMMY) * 8 + (menv[monster_killed].m_class == MONS_MUMMY_PRIEST) * 5, random2(30) + random2(30) + random2(30), 100);
        }
    }

    if (menv[monster_killed].m_class == MONS_WORM_TAIL)
    {
        dmi = monster_killed;
        while (menv[dmi].m_class != MONS_TUNNELING_WORM)
        {
            dmi--;
            if ((menv[dmi].m_class != MONS_WORM_TAIL && menv[dmi].m_class != MONS_TUNNELING_WORM) || dmi < 0)
            {
                strcpy(info, "Error: Worm head not found!");
                mpr(info);
                strcpy(info, "Oh well. Carry on.");
                mpr(info);
                goto out_of_worm;
            }
        }

        if (menv[dmi].m_HD >= 3)
        {
            menv[dmi].m_HD -= 2;
            menv[dmi].m_hp_max -= 8 + random2(8);
            menv[dmi].m_hp -= random2(8) + 8;
            if (menv[dmi].m_hp_max <= 0)
                menv[dmi].m_hp_max = 1;
            if (menv[dmi].m_hp > menv[dmi].m_hp_max)
                menv[dmi].m_hp = menv[dmi].m_hp_max;
            if (menv[dmi].m_hp <= 0)
                menv[dmi].m_hp = 1;
        }
    }


out_of_worm:
    if (menv[monster_killed].m_class == MONS_TUNNELING_WORM || menv[monster_killed].m_class == MONS_WORM_TAIL)
    {
        dmi = monster_killed;

        while (menv[dmi + 1].m_class == MONS_WORM_TAIL)
        {
            dmi++;
            menv[dmi].m_class = -1;
            mgrd[menv[dmi].m_x][menv[dmi].m_y] = MNG;
        }

    }


    if (killer != 6)
    {
        if (menv[monster_killed].m_ench[1] >= 20 && menv[monster_killed].m_ench[1] <= 25)
        {
            if (mons_near(monster_killed) && mons_weight(mons_charclass(menv[monster_killed].m_class)) != 0)
            {
                strcpy(info, monam(menv[monster_killed].m_sec, menv[monster_killed].m_class, menv[monster_killed].m_ench[2], 0));
                strcat(info, "'s corpse disappears in a puff of smoke!");
                mpr(info);
                place_cloud(CLOUD_GREY_SMOKE_MON + random2(3), menv[monster_killed].m_x, menv[monster_killed].m_y, 1 + random2(3));
            }
        }
        else
            place_monster_corpse(monster_killed);
    }

    monster_drop_ething(monster_killed);

    int j;

    for (j = 0; j < 3; j++)
    {
        menv[monster_killed].m_ench[j] = 0;
    }

    menv[monster_killed].m_ench_1 = 0;

    /*no_mons --; */
    menv[monster_killed].m_class = -1;
    menv[monster_killed].m_hp = 0;
    menv[monster_killed].m_hp_max = 0;
    menv[monster_killed].m_HD = 0;
    menv[monster_killed].m_AC = 0;
    menv[monster_killed].m_ev = 0;

    mgrd[menv[monster_killed].m_x][menv[monster_killed].m_y] = MNG;

    for (dmi = 7; dmi >= 0; dmi--)
    {
        menv[monster_killed].m_inv[dmi] = ING;
    }


    /*brek = 1; */
    viewwindow(1);

    for (dmi = 0; dmi < MNST; dmi++)
    {
        if (menv[dmi].m_hit == monster_killed)
            menv[dmi].m_hit = MHITNOT;
    }

    if (you[0].pet_target == monster_killed)
        you[0].pet_target = MHITNOT;

    return;
}



int jelly_divide(int jel)
{

    int k = 0;

    if (menv[jel].m_class != MONS_JELLY)
        return 0;

    if (menv[jel].m_hp == 1)
        return 0;

    int jex = 0;
    int jey = 0;

    for (jex = -1; jex < 3; jex++)
    {

        if (jex == 2)
            return 0;

        for (jey = -1; jey < 2; jey++)
        {
            if (mgrd[menv[jel].m_x + jex][menv[jel].m_y + jey] == MNG && mgrd[menv[jel].m_x + jex][menv[jel].m_y + jey] > 10 && (menv[jel].m_x + jex != you[0].x_pos || menv[jel].m_y + jey != you[0].y_pos))
                /* 10-50 for now - must take clouds into account */
                goto jel2;
        }                       /* end of for jey */

    }                           /* end of for jex */

  jel2:for (k = 0; k < MNST; k++)
    {
        if (k == MNST - 1)
            return 0;

        if (menv[k].m_class == -1)
            break;
    }

    menv[jel].m_hp_max /= 2;
    if (menv[jel].m_hp > menv[jel].m_hp_max)
        menv[jel].m_hp = menv[jel].m_hp_max;

    menv[k].m_class = MONS_JELLY;
    menv[k].m_HD = 4;
    menv[k].m_hp = menv[jel].m_hp;
    menv[k].m_hp_max = menv[k].m_hp;
    menv[k].m_AC = 0;
    menv[k].m_ev = 2;
    menv[k].m_speed = 5;
    menv[k].m_speed_inc = 70 + random2(5);

    menv[k].m_x = menv[jel].m_x + jex;
    menv[k].m_y = menv[jel].m_y + jey;

    mgrd[menv[k].m_x][menv[k].m_y] = k;

/*         no_mons ++;
   mn_no ++; */

    menv[k].m_beh = menv[jel].m_beh;    /* Look at this! */

    if (mons_near(jel))
    {
        strcpy(info, "The jelly splits in two!");
        mpr(info);
    }
    else
    {
        strcpy(info, "You hear a squelching noise in the distance.");
        mpr(info);
    }

    return 1;

}



void alert(void)
{
/* if you're invis and throw/zap whatever, alerts menv to your position */

    int it;

    for (it = 0; it < MNST; it++)
    {

        if (menv[it].m_class == -1)
            continue;

        if (mons_near(it))
        {
            if (menv[it].m_beh == BEH_CHASING_I || menv[it].m_beh == BEH_WANDER || menv[it].m_beh == BEH_SLEEP)
            {
                menv[it].m_beh = BEH_CHASING_I;
                if (you[0].invis == 0 || mons_see_invis(menv[it].m_class) == 1)
                {
                    menv[it].m_targ_1_x = you[0].x_pos;
                    menv[it].m_targ_1_y = you[0].y_pos;
                }
            }
        }

    }                           /* end of for it */

}                               /* end of void alert() */




void monster_polymorph(unsigned char monsc, unsigned char targetc, int power)
{

    int k = power;

    k = 0;                      /* to avoid that annoying warning */

    if (targetc == 250)
    {
loopy:
        do
        {
            targetc = random2(400);
        }
        while (mons_rarity(targetc) == 0 || targetc == 99 || targetc == 25 || targetc == 51 || targetc == 367 || targetc == 107 || targetc == 108);     /* no shapeshifters or zombies/skeletons/spectr */

        if (grd[menv[monsc].m_x][menv[monsc].m_y] == 61 || grd[menv[monsc].m_x][menv[monsc].m_y] == 62)
            if (mons_flies(targetc) == 0)
                goto loopy;     /* Not fair to instakill a monster like this (actually, I can't be bothered implementing it) */
        /* Too long to put in the loop thing */
    }

/* if (power != -1) // automatic success */

    unsigned char old_class = menv[monsc].m_class;
    unsigned char old_hp = menv[monsc].m_hp;
    unsigned char old_hp_max = menv[monsc].m_hp_max;
    unsigned char old_sec = menv[monsc].m_sec;

/* deal with mons_sec */


    menv[monsc].m_class = targetc;


    menv[monsc].m_sec = 250;

    k = monsc;

    int unenc = 0;

    for (unenc = 0; unenc < 3; unenc++)
    {
        if (menv[monsc].m_ench[unenc] >= 20 && menv[monsc].m_ench[unenc] <= 25)
            continue;           /* Summoned creatures are still going to disappear eventually */
        if (menv[monsc].m_ench[unenc] != 39 && menv[monsc].m_ench[unenc] != 38)
            menv[monsc].m_ench[unenc] = 0;      /* shapeshifters stay as such */
    }

    if (mons_flag(menv[monsc].m_class, M_INVIS))
    {
        menv[monsc].m_ench_1 = 1;
        menv[monsc].m_ench[2] = 6;
    }

    if (menv[monsc].m_ench[0] == 0 && menv[monsc].m_ench[1] == 0 && menv[monsc].m_ench[2] == 0)
        menv[monsc].m_ench_1 = 0;

    define_monster(monsc, menv);

    menv[monsc].m_hp = menv[monsc].m_hp_max * ((old_hp * 100) / old_hp_max) / 100;
    menv[monsc].m_speed_inc = 70 + random2(5);

/* make sure that eg manticores don't drop their darts */

    monster_drop_ething(monsc);

    strcpy(info, monam(old_sec, old_class, menv[monsc].m_ench[2], 0));
    if (menv[monsc].m_ench[1] == 39 || menv[monsc].m_ench[1] == 38)
        strcat(info, " changes into ");
    else if (targetc == 131)
        strcat(info, " degenerates into ");
    else
        strcat(info, " evaporates, and reforms as ");

    strcat(info, monam(menv[monsc].m_sec, menv[monsc].m_class, menv[monsc].m_ench[2], 3));
    if (targetc == 131)
        strcat(info, " of flesh");
    strcat(info, "!");
    if (mons_near(monsc))
        mpr(info);

}                               /* end of monster_polymorph */


void monster_drop_ething(int monster_killed)
{
/* use monster_killed for convenience
   drop weapons & missiles last (ie on top) so others pick up */

    int ygy;
    char splashes = 0;

    if (grd[menv[monster_killed].m_x][menv[monster_killed].m_y] == 61 || grd[menv[monster_killed].m_x][menv[monster_killed].m_y] == 62)
    {
        for (ygy = 0; ygy < 8; ygy++)
        {
            if (menv[monster_killed].m_inv[ygy] != ING)
            {
                destroy_item(menv[monster_killed].m_inv[ygy]);
                splashes++;
            }
        }
        if (splashes > 0)
        {
            strcpy(info, "You hear a splashing sound.");
            mpr(info);
        }
        return;
    }

    for (ygy = 7; ygy >= 0; ygy--)
    {
        if (menv[monster_killed].m_inv[ygy] != ING)
        {
            mitm.ilink[menv[monster_killed].m_inv[ygy]] = ING;
            if (igrd[menv[monster_killed].m_x][menv[monster_killed].m_y] == ING)
                /* this bit is wrong, BTW:
                   if (mons_inv [monster_killed] [j] != ING) */
            {
                igrd[menv[monster_killed].m_x][menv[monster_killed].m_y] = menv[monster_killed].m_inv[ygy];
            }
            else
            {
                int huj = igrd[menv[monster_killed].m_x][menv[monster_killed].m_y];

                igrd[menv[monster_killed].m_x][menv[monster_killed].m_y] = menv[monster_killed].m_inv[ygy];
                mitm.ilink[menv[monster_killed].m_inv[ygy]] = huj;
            }
        }


        menv[monster_killed].m_inv[ygy] = ING;
    }



}                               /* end of void monster_drop_ething */



void place_monster_corpse(unsigned char mcr)
{
    int corpse_class = mons_charclass(menv[mcr].m_class);

    if (menv[mcr].m_ench[1] == 39)
        corpse_class = MONS_SHAPESHIFTER;       /* shapeshifter */
    if (menv[mcr].m_ench[1] == 38)
        corpse_class = MONS_GLOWING_SHAPESHIFTER;       /* shapeshifter */

    if (mons_weight(corpse_class) == 0 || coinflip() )
        return;
    if (grd[menv[mcr].m_x][menv[mcr].m_y] == 61 || grd[menv[mcr].m_x][menv[mcr].m_y] == 62)
        return;

    int o;

    for (o = 0; o < ITEMS; o++)
    {
        if (o >= 200 + random2(150))
            return;
        if (mitm.iquant[o] == 0)
        {
            mitm.iid[o] = 0;
            mitm.iclass[o] = OBJ_CORPSES;
            mitm.iplus[o] = corpse_class;
            mitm.iplus2[o] = 0;
            mitm.itype[o] = CORPSE_BODY;
            mitm.idam[o] = 210;
            mitm.icol[o] = mcolour[corpse_class];
            if (mcolour[corpse_class] == BLACK)
                mitm.icol[o] = menv[mcr].m_sec;
            mitm.iquant[o] = 1;
            mitm.ilink[o] = ING;
            break;
        }
    }                           // end of o loop

    if (igrd[menv[mcr].m_x][menv[mcr].m_y] == ING)
    {
        igrd[menv[mcr].m_x][menv[mcr].m_y] = o;
    }
    else
    {
        int hug = igrd[menv[mcr].m_x][menv[mcr].m_y];

        igrd[menv[mcr].m_x][menv[mcr].m_y] = o;
        mitm.ilink[o] = hug;
    }

    return;

}
