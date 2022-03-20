#include <stdio.h>

#include "mtm.h"

// Forward declarations - platform-specific
void usage(char *pname);

// The following handle any necessary forward declared definitions
#ifndef MTM_GUI
#include "mtm_unix_cli.c"
#else
#include "mtm_unix_gui.c"
#endif

int main(int argc, char **argv)
{
    if (argc != 1) {
        usage(argv[0]);
        exit(1);
    }

    init_ui();
    run();
    teardown_ui();

    return 0;
}

void usage(char *pname)
{
    printf("Usage: %s\n", pname);
}
