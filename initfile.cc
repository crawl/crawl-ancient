/*
 *  File:       initfile.cc
 *  Summary:    Simple reading of an init file and system variables
 *  Written by: David Loewenstern
 *
 *  Change History (most recent first):
 *
 *      <2>     6/12/99         BWR             Added get_system_environment
 *      <1>     6/9/99          DML             Created
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AppHdr.h"
#include "initfile.h"
#include "enum.h"
#include "externs.h"
#include "items.h"


char *obj_syms = ")([/%.?=!.+\\0}X$";
int obj_syms_len = 16;


void read_init_file()
{
    FILE *f;
    char s[255], field[100];
    int i, j, line = 0;

    you.your_name[0] = '\0';

    if (sys_env.crawl_rc)
    {
        f = fopen(sys_env.crawl_rc, "r" );
    }
    else if (sys_env.crawl_dir)
    {
        char name_buff [kPathLen];

        strncpy( name_buff, sys_env.crawl_dir, kPathLen );
        strncat( name_buff, "init.txt", kPathLen );

        f = fopen( name_buff, "r" );
    }
    else
    {
        f = fopen( "init.txt", "r" );
    }

    if (f == NULL)
        return;

    while (!feof(f))
    {
        fgets(s, 255, f);

        // This is to make some efficient comments
        if (s[0] == '#' || s[0] == '\0')  continue;

        line++;

        // everything not a valid line is treated as a comment
        if (sscanf(s, "autopickup=%s", field))
        {
            for (i = 0; i < 100 && field[i] != '\0'; i++)
            {
                // Make the amulet symbol equiv to ring -- bwross
                switch (field[i])
                {
                    case '"':
                        // also represents jewellery
                        field[i] = '=';
                        break;

                    case '|':
                        // also represents staves
                        field[i] = '\\';
                        break;

                    case ':':
                        // Also represents books
                        field[i] = '+';
                        break;

                    case 'x':
                        // also corpses
                        field[i] = 'X';
                        break;
                }

                for (j = 0; j < obj_syms_len && field[i] != obj_syms[j]; j++)
                    ;

                if (j < obj_syms_len)
                    autopickups |= (1L << j);
                else
                    fprintf(stderr, "Unparseable line #%d in init file", line);
            }
        }
        else if (sscanf(s, "name=%s", field))
        {
            strncpy( you.your_name, field, kNameLen );
        }
        else if (sscanf(s, "crawl_dir=%s", field))
        {
            // We shouldn't bother to allocate this a second time
            // if the user puts two crawl_dir lines in his init file.
            if (!sys_env.crawl_dir)
                sys_env.crawl_dir = (char *) calloc( kPathLen, sizeof(char) );

            if (sys_env.crawl_dir)
                strncpy( sys_env.crawl_dir, field, kNameLen );
        }
    }
    fclose(f);
}


void get_system_environment()
{
    // The player's name
    sys_env.crawl_name = getenv( "CRAWL_NAME" );

    // The player's pizza
    sys_env.crawl_pizza = getenv( "CRAWL_PIZZA" );

    // The directory which contians init.txt, macro.txt, morgue.txt
    // This should end with the appropriate path delimiter.
    sys_env.crawl_dir = getenv( "CRAWL_DIR");

    // The full path to the init file -- this over-rides CRAWL_DIR
    sys_env.crawl_rc = getenv( "CRAWL_RC" );

}
