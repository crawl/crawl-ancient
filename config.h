// No need to edit anymore; Will auto-select OS.
// (If not on Linux but on another *nix you might
// want to force LINUX however)

#define DOS


/*
Define plain_term for linux and similar, and dos_term for DOS and EMX.
*/
#ifdef linux
  #undef DOS
  #define LINUX
  #define PLAIN_TERM
  #include "linuxlib.h"
#endif


/*
To compile with EMX for OS/2 define USE_EMX macro with compiler command line
(already defined in supplied makefile.emx)
*/
#ifdef USE_EMX
  #undef DOS
  #define DOS_TERM
  #include "libemx.h"
#endif


/*
The first EOL should be used for DOS; most other OSs apparently
use \n
*/
#ifdef DOS
  #define EOL "\n\r"
  #define DOS_TERM
#else
  #define EOL "\n"
#endif


/*
Define MACRO if you want to use the macro patch in macro.cc.
*/
#define MACROS
