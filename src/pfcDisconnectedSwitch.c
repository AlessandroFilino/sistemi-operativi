#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include "../include/pfcDisconnectedSwitch.h"
#include "../include/utility.h"
#include "../include/path.h"

int main(int argc, const char *argv[]) {
    //char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";

    int pid;
    int pfcNumber;
    int pfcProcess[3];
    int generatoreFallimentiProcess;

    int read;
    char *error = calloc(14 + 1, sizeof(char));
    unlink(FILENAME_WES_PIPE);
    mknod(FILENAME_WES_PIPE, S_IFIFO, 0);
    chmod(FILENAME_WES_PIPE, 0660);

    //TODO pipe bloccante?
    int wesPipe = open(FILENAME_WES_PIPE, O_RDONLY | O_NONBLOCK);
    FILE *switchLog = open_file(FILENAME_SWITCH_LOG, "w");

    /*
     * TODO: signal(SIGCHLD, SIG_IGN); (ignore death-of-child signals to prevent zombies)
     *       oppure
     *       signal (SIGCHLD, childHandler); (Install death-of-child handler)
     *       ?
     */
    signal (SIGCHLD, childHandler);

    char *PFC1_argv[] = {"pfc1", filename_g18};
    pfcProcess[0] = createChild(&execv, "pfc1", PFC1_argv);

    char *PFC2_argv[] = {"pfc2", filename_g18};
    pfcProcess[1] = createChild(&execv, "pfc2", PFC2_argv);

    char *PFC3_argv[] = {"pfc3", filename_g18};
    pfcProcess[2] = createChild(&execv, "pfc3", PFC3_argv);

    char *pfc1Process = malloc(sizeof(char) * 10);
    char *pfc2Process = malloc(sizeof(char) * 10);
    char *pfc3Process = malloc(sizeof(char) * 10);
    intToString(pfc1Process, 10, pfcProcess[0]);
    intToString(pfc2Process, 10, pfcProcess[1]);
    intToString(pfc3Process, 10, pfcProcess[2]);

    char *generatoreFallimenti_argv[] = {"generatoreFallimenti", pfc1Process, pfc2Process, pfc3Process};
    generatoreFallimentiProcess = createChild(&execv, "generatoreFallimenti_argv", generatoreFallimenti_argv);

    char **pfcArgv[] = {PFC1_argv, PFC2_argv, PFC3_argv};

    for(int i=0; i<50; i++) {
        read = readLine(wesPipe, error, '\0');

        //TODO se la pipe è vuota, readLine dovrebbe restituire 0 o -1
        if(read != 0) {
            if(strcmp(error, "EMERGENZA") == 0) {
                kill(pfcProcess[0], SIGINT);
                kill(pfcProcess[1], SIGINT);
                kill(pfcProcess[2], SIGINT);
            } else {
                pfcNumber = getErrorInfo(error);
                pid = pfcProcess[pfcNumber - 1];

                /*
                 * kill(pidFiglio, 0) restituisce 0 se esiste un processo
                 * con pid uguale a pidFiglio, altrimenti -1
                 */
                if(kill(pid, 0) == 0) {
                    //il processo esiste

                    kill(pid, SIGCONT);
                    fprintf(switchLog, "Iniviato un segnale SIGCONT a PFC%d\n", pfcNumber);
                } else {
                    //il processo non esiste più

                    char filename[4];
                    sprintf(filename, "pfc%d", pfcNumber);

                    pfcProcess[pfcNumber - 1] = createChild(&execv, filename, pfcArgv[pfcNumber - 1]);
                    fprintf(switchLog, "PFC%d è stato ricreato\n", pfcNumber);
                }
            }
        }
    }

    close(wesPipe);

    return 0;
}

void childHandler(int sig) { /* Executed if the child dies */
    int childPid, childStatus; /* before the parent */

    /* Accept child termination code */
    childPid = wait(&childStatus);
    printf ("Child %d terminated\n", childPid);

    exit(EXIT_SUCCESS);
}

int getErrorInfo(char *error) {
    char *buffer[2];

    for(int i=0; i<2; i++) {
        buffer[i] = calloc(10, sizeof(char));
    }

    if(!tokenize(error, "-", 2, buffer)) {
        return -1;
    }

    int counter = 9;
    char *pfcInfo = buffer[1];
    while(pfcInfo[counter] == '\0') {
        counter--;
    }

    return pfcInfo[counter];
}

