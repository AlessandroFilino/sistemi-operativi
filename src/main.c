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
    int status;
    int pid;
    char filename_G18[15] = {0};
    strcpy(filename_G18, argv[1]);

    if(system(remove_files_from(PATHNAME_TEMP)) || system(remove_files_from(PATHNAME_LOG)) != 0) {
        fprintf(stderr, "%s\n", MAIN_PATH_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    }

    if(access(filename_G18, F_OK) == -1) {
        fprintf(stderr, "%s\n", MAIN_INPUT_FNF_MESSAGE);
        exit(EXIT_FAILURE);
    }

    createEmptyFile(FILENAME_LAST_READ, "w");

    char* transducers_argv[] = {"transducers", NULL};
    int pid_transducers = createChild(&execv, "transducers", transducers_argv);

    char* pfc1_argv[] = {"pfc1", filename_G18, NULL};
    int pid_pfc1 = createChild(&execv, "pfc1", pfc1_argv);

    char* pfc2_argv[] = {"pfc2", filename_G18, NULL};
    int pid_pfc2 = createChild(&execv, "pfc2", pfc2_argv);

    char* pfc3_argv[] = {"pfc3", filename_G18, NULL};
    int pid_pfc3 = createChild(&execv, "pfc3", pfc3_argv);

    for(int i=0; i<4; i++) {
        pid = wait(&status);

        if (WIFEXITED(status)) {

            /* Child process exited normally, through `return` or `exit` */
            printf("Child process %d exited with %d status\n", pid, WEXITSTATUS(status));
        }
    }

    printf("transducers: %d\npfc1: %d\npfc2: %d\npfc3: %d\n", pid_transducers, pid_pfc1, pid_pfc2, pid_pfc3);

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


