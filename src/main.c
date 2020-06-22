#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utility.h"

int main(int argc, const char* argv[]) {
    /*
     * argv[1] --> g18.txt
     */

    int status;
    char *filename = "../sistemioperativi/doc/G18.txt";

    char *pfcDisconnectedSwitch_argv[] = {"pfcDisconnectedSwitch", filename, NULL};
    createChild(&execv, "pfcDisconnectedSwitch", pfcDisconnectedSwitch_argv);

    char *transducer_argv[] = {"transducer", NULL};
    createChild(&execv, "transducer", transducer_argv);

    char *wes_argv[] = {"wes", NULL};
    createChild(&execv, "wes", wes_argv);

    for(int i=0; i<2; i++) {
        wait(&status);
    }

    return 0;
}
