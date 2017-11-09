/* picoc main program - this varies depending on your operating system and
 * how you're using picoc */
 
/* include only picoc.h here - should be able to use it with only the external interfaces, no internals from interpreter.h */
#include "picoc.h"

/* platform-dependent code for running programs is in this file */

#if defined(UNIX_HOST) || defined(WIN32)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PICOC_STACK_SIZE (128*1024)              /* space for the the stack */

int main(int argc, char **argv)
{
    int DontRunMain = FALSE;
    int StackSize = getenv("STACKSIZE") ? atoi(getenv("STACKSIZE")) : PICOC_STACK_SIZE;
    Picoc pc;
    
    PicocInitialise(&pc, StackSize);
    if (argc < 2)
    {
        PicocIncludeAllSystemHeaders(&pc);
        PicocParseInteractive(&pc);
    }
    else if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-m") == 0) 
    {
        DontRunMain = TRUE;
        PicocIncludeAllSystemHeaders(&pc);
    } 
    else 
    {
        printf("Format: picoc : default interactive mode (including all sys headers)\n"
               "        picoc -s <csource1.c>... [- <arg1>...] : script mode - runs the program without calling main()\n");
        exit(1);
    }

    PicocCleanup(&pc);
    return pc.PicocExitValue;
}
#else
# ifdef SURVEYOR_HOST
#  define HEAP_SIZE C_HEAPSIZE
#  include <setjmp.h>
#  include "../srv.h"
#  include "../print.h"
#  include "../string.h"

int picoc(char *SourceStr)
{   
    char *pos;

    PicocInitialise(HEAP_SIZE);

    if (SourceStr)
    {
        for (pos = SourceStr; *pos != 0; pos++)
        {
            if (*pos == 0x1a)
            {
                *pos = 0x20;
            }
        }
    }

    PicocExitBuf[40] = 0;
    PicocPlatformSetExitPoint();
    if (PicocExitBuf[40]) {
        printf("Leaving PicoC\n\r");
        PicocCleanup();
        return PicocExitValue;
    }

    if (SourceStr)   
        PicocParse("nofile", SourceStr, strlen(SourceStr), TRUE, TRUE, FALSE);

    PicocParseInteractive();
    PicocCleanup();
    
    return PicocExitValue;
}
# endif
#endif
