switch (show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9]) // was + 7
        {

/*         buffy [bufcount] = show [count_x - you[0].x_pos + 7] [count_y - you[0].y_pos + 7];
         buffy [bufcount + 1] = show_col [count_x - you[0].x_pos + 7] [count_y - you[0].y_pos + 7]; //MAGENTA;

         bufcount += 2;*/

         //if (bufcount > 5) abort();


                case 0: showed = 0; break;
                case 1: buffy [bufcount + 1] = rock_colour; //BROWN;
                showed = 177; break; // rock wall - remember earth elementals
                case 2: buffy [bufcount + 1] = LIGHTGRAY;
                showed = 177; break; // stone wall
                //                ^^^^^
                // later have level variables for each of them, as LIGHTGRAY walls may look slightly silly in hell

                case 3: showed = 254; break; //43; break;
  case 4: showed = 177;
  buffy [bufcount + 1] = CYAN; break;
  case 5: showed = 177; // hidden secret door
  buffy [bufcount + 1] = rock_colour; break;
  case 6: showed = 177;
  buffy [bufcount + 1] = GREEN; break; // green crystal wall
  case 7: showed = '1';
  buffy [bufcount + 1] = DARKGRAY; // orcish idol
  break;

  case 21: showed = '1';
  buffy [bufcount + 1] = WHITE; // silver statue
  break;
  case 22: showed = 219; // filled square
  buffy [bufcount + 1] = LIGHTGRAY; // granite monolith
  break;

case 35: showed = '#'; break;

  case 61: showed = 247;
  buffy [bufcount + 1] = RED; // lava!
  break;
  case 62: showed = 247; // this wavy thing also used for water elemental
  buffy [bufcount + 1] = BLUE; // water
  break;

                case 67: buffy [bufcount + 1] = floor_colour; //LIGHTGRAY;
                showed = 249; break;
//              case 68: showed = '>'; break; //60; break; // <
                case 69: showed = 239; buffy [bufcount + 1] = RED; break; // staircase to hell
                case 70: showed = 39; break; // open door
  case 71: showed = 240; buffy [bufcount + 1] = BROWN; break; // branch staircase
//              case 72: showed = '<'; break; //60; break; // <

       /*               case 22: showed = 62; break;
                case 23: showed = 60; break;
                case 24: showed = 62; break;*/
                case 75: buffy [bufcount + 1] = 11;
                showed = 94; break; // ^ dart trap
                case 76: buffy [bufcount + 1] = MAGENTA;
                showed = 94; break;
                case 77: buffy [bufcount + 1] = LIGHTGRAY;
                showed = 94; break;
                case 78: showed = 249; buffy [bufcount + 1] = floor_colour;
  break; // undiscovered trap

  case 80: showed = 239; buffy [bufcount + 1] = YELLOW;
       break;
// if I change anything above here, must also change magic mapping!
  case 81: showed = 239; buffy [bufcount + 1] = LIGHTGRAY; break; // labyrinth

  case 82:
  case 83:
  case 84:
  case 85:
       showed = '>'; break;

  case 86:
  case 87:
  case 88:
  case 89:
       showed = '<'; break;

  case 92: buffy [bufcount + 1] = CYAN; showed = 239; break; // Stairway to Dis
  case 93: buffy [bufcount + 1] = RED; showed = 239; break; // Gehenna
  case 94: buffy [bufcount + 1] = LIGHTCYAN; showed = 239; break; // Cocytus
  case 95: buffy [bufcount + 1] = DARKGRAY; showed = 239; break; // Tartarus
  case 96: buffy [bufcount + 1] = LIGHTMAGENTA; showed = 239; break; // To Abyss
  case 97: buffy [bufcount + 1] = LIGHTMAGENTA; showed = 239; break; // From Abyss
  case 98: buffy [bufcount + 1] = LIGHTGRAY; showed = 239; break; // Closed gate to hell
  case 99: buffy [bufcount + 1] = LIGHTBLUE; showed = 239; break; // gate to pandemonium
  case 100: buffy [bufcount + 1] = LIGHTBLUE; showed = 239; break; // gate out of pandemonium
  case 101: buffy [bufcount + 1] = LIGHTGREEN; showed = 239; break; // gate to other part of pandemonium


                case 256: showed = 9; break;

                case 258: showed = 41; break; // weapon )
                case 259: showed = 91; break; // armour [
                case 260: showed = 47; break; // food %
                case 261: showed = 37; break; // wands / &c
                case 262: showed = 43; break; // books +
                case 263: showed = 63; break; // scroll ?
                case 264: showed = 61; break; // ring = etc
                case 265: showed = 33; break; // potions !
                case 266: showed = 40; break; // stones
                case 267: showed = '+'; break; // book +
                case 268: showed = 37; break; // corpses part 1
  case 269: showed = '\\'; break; // magical staves
  case 270: showed = '}'; break; // gems
  case 271: showed = '%'; break; // don't know ?
                case 272: showed = 36; break; // $ gold


//  case 273: showed = '}'; break; // Misc

default:
        int mnr=show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9];
        if(mnr>=297) {
                showed=mons_char(mnr-297); // yeah
        } else
                showed = show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9]; break;



                }
