/*
 *  File:       defines.h
 *  Summary:    Various definess used by Crawl.
 *  Written by: Linlet Henzel
 *
 *      Abstract:       A variety of miscellaneous constant values are found here.
 *                      I think we should move the colors into an enum or something
 *                      because there are in numerical order.  But I'm too lazy to
 *                      do it myself.
 *
 *  Copyright © 1999 Brian Robinson.
 *
 *  Change History (most recent first):
 *
 *       <2>     6/17/99        BCR             indented and added header
 *       <1>     --/--/--       LRH             Created
 */

#ifndef DEFINES_H
  #define DEFINES_H

  #define NO_EQUIP 10

  // there's got to be a better way...
  #ifdef _LINUXLIB_IMPLEMENTATION
  #elif macintosh
  #else

    #ifndef TRUE
      #define TRUE 1
    #endif

    #ifndef FALSE
      #define FALSE 0
    #endif

  #endif

  #define MNST 200
  #define MNG 201
  #define ITEMS 500
  #define ING 501
  /* max clouds on lvl */
  #define CLOUDS 30
  /* an empty cloud */
  #define CNG 31
  /* Unused: */
  #define GXM 80
  #define GYM 70
  /* max traps on level */
  #define NTRAPS 30
  /* Lowest grid value which can be passed by walking etc: */
  #define MINMOVE 31
  /* Lowest grid value which can be seen through */
  #define MINSEE 11


  #define menv env.mons
  #define mitm env.it[0]
  #define grd env.grid
  #define mgrd env.mgrid
  #define igrd env.igrid

  #define MHITNOT 201
  #define MHITYOU 202

  #define MLAVA0 420
  #define MLAVA1 421
  #define MLAVA2 422
  #define MLAVA3 423
  #define MLAVA4 424
  #define MLAVA5 425
  #define MLAVA6 426
  #define MLAVA7 427
  #define MLAVA8 428
  #define MLAVA10 429
  #define MWATER0 430
  #define MWATER1 431
  #define MWATER2 432
  #define MWATER3 433
  #define MWATER4 434
  #define MWATER5 435
  #define MWATER6 436
  #define MWATER7 437
  #define MWATER8 438
  #define MWATER9 439
  #define MWATER10 440

  #ifndef DOS
    #define BLACK 0
    #define BLUE 1
    #define GREEN 2
    #define CYAN 3
    #define RED 4
    #define MAGENTA 5
    #define BROWN 6
    #define LIGHTGREY 7
    #define DARKGREY 8
    #define LIGHTBLUE 9
    #define LIGHTGREEN 10
    #define LIGHTCYAN 11
    #define LIGHTRED 12
    #define LIGHTMAGENTA 13
    #define YELLOW 14
    #define WHITE 15
  #else
    #include <conio.h>
    #define LIGHTGREY LIGHTGRAY
    #define DARKGREY DARKGRAY
  #endif

#endif // DEFINES_H
