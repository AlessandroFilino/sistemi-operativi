#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/utility.h"
#include "../include/main.h"
#include "../include/path.h"
#include "../include/messages.h"

int main(int argc, const char* argv[]) {
    /*
     * argv[1] --> g18.txt
     */
    int status;
    char *filename_G18 = "../doc/G18.txt";

    if(system(remove_files_from(PATHNAME_TEMP)) || system(remove_files_from(PATHNAME_LOG)) != 0) {
        fprintf(stderr, "%s", MAIN_PATH_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    }

    if(access(filename_G18, F_OK) == -1) {
        fprintf(stderr, "%s", MAIN_INPUT_FNF_MESSAGE);
        exit(EXIT_FAILURE);
    }

    //int fd_p1 = open(p1, O_WRONLY | O_NONBLOCK, 0660);
    //printf("(%s) %d\n", strerror(errno), ENXIO);

    //int fd_p2 = open(p2, O_WRONLY | O_NONBLOCK, 0660);
    //printf("(%s)\n", strerror(errno));

    /*char *pfcDisconnectedSwitch_argv[] = {"pfcDisconnectedSwitch", filename_G18, NULL};
    createChild(&execv, "pfcDisconnectedSwitch", pfcDisconnectedSwitch_argv);

    char *transducer_argv[] = {"transducer", NULL};
    createChild(&execv, "transducer", transducer_argv);

    char *wes_argv[] = {"wes", NULL};
    createChild(&execv, "wes", wes_argv);

    for(int i=0; i<2; i++) {
        wait(&status);
    }*/

    return 0;
}


