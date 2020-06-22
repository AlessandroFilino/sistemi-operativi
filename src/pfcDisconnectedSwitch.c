#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "pfcDisconnectedSwitch.h"
#include "utility.h"

int main(int argc, const char *argv[]) {
    //char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";

    int pfcProcess[3];
    int generatoreFallimentiProcess;

    /*Install death-of-child handler */
    signal (SIGCHLD, childHandler);

    char *PFC1_argv[] = {"pfc1", filename_g18};
    pfcProcess[0] = createChild(&execv, "pfc1", PFC1_argv);

    /*char *PFC2_argv[] = {"pfc2", filename_g18};
    pfcProcess[1] = createChild(&execv, "pfc2", PFC2_argv);

    char *PFC3_argv[] = {"pfc3", filename_g18};
    pfcProcess[2] = createChild(&execv, "pfc3", PFC3_argv);

    char *generatoreFallimenti_argv[] = {
            "generatoreFallimenti",
            intToString(pfcProcess[0]),
            intToString(pfcProcess[1]),
            intToString(pfcProcess[2])};
    generatoreFallimentiProcess = createChild(&execv, "generatoreFallimenti_argv", generatoreFallimenti_argv);


    for(;;) {
        //execlp(...);
        //signal(SIGCHLD, SIG_IGN); //ignore death-of-child signals to prevent zombies
        //kill(pidFiglio, SIGN_INT) per tutti i figli
        //kill(pidFiglio, 0) restituisce 0 se esiste un processo con pid uguale a pidFiglio, altrimenti -1
    }*/

    return 0;
}

void childHandler(int sig) { /* Executed if the child dies */
    int childPid, childStatus; /* before the parent */

    /* Accept child termination code */
    childPid = wait(&childStatus);
    printf ("Child %d terminated\n", childPid);

    exit(EXIT_SUCCESS);
}

