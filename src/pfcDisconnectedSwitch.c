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
#include "../include/errors.h"

int main(int argc, const char *argv[]) {
    //char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";

    int pid_pfc;
    int pfcNumber;
    int pfcProcess[3];
    int generatoreFallimentiProcess;

    int read;
    char error[WES_MESSAGE_MAX_LENGTH] = {0};

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

    char *PFC1_argv[] = {"pfc1", filename_g18, NULL};
    pfcProcess[0] = createChild(&execv, "pfc1", PFC1_argv);

    char *PFC2_argv[] = {"pfc2", filename_g18, NULL};
    pfcProcess[1] = createChild(&execv, "pfc2", PFC2_argv);

    char *PFC3_argv[] = {"pfc3", filename_g18, NULL};
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

        /*
         * se la pipe è vuota, readLine restituisce 0
         * se avviene un errore durante la lettura, readLine restituisce -1
         */
        if(read > 0) {
            if(strcmp(error, WES_MESSAGE_EMERGENCY) == 0) {
                kill(pfcProcess[0], SIGUSR2);
                kill(pfcProcess[1], SIGUSR2);
                kill(pfcProcess[2], SIGUSR2);

                fprintf(switchLog, "SIGUSR2 sent to PFC1, PFC2, PFC3\n");
            } else {
                pfcNumber = getErrorInfo(error);
                pid_pfc = pfcProcess[pfcNumber - 1];

                /*
                 * kill(pidFiglio, 0) restituisce 0 se esiste un processo
                 * con pid_pfc uguale a pidFiglio, altrimenti -1
                 */
                if(kill(pid_pfc, 0) == 0) {
                    //il processo esiste

                    //TODO controllare se il processo è bloccato oppure no
                    kill(pid_pfc, SIGCONT);
                    fprintf(switchLog, "SIGCONT sent to PFC%d\n", pfcNumber);
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
    unsigned int size = (unsigned int) strlen(error);

    int result = 0;
    char *endPointer = &error[size-1];

    while(*endPointer >= '0' && *endPointer <= '9') {
        result = result * 10 + (*endPointer - '0');
        endPointer--;
    }

    return result;
}

/*
 * Con waitpid() e WIFSTOPPED() è possibile controllare lo stato di un processo
 *
    pid_t pid = fork();
    if(pid == 0) {
        printf("child\n");
        fflush(stdout);
        usleep((1 * 1000) * 1000 * 3);
        printf("exit");
        exit(EXIT_SUCCESS);
    } else {
        usleep((1 * 1000) * 2000);
        kill(pid, SIGSTOP);
        int test_status;
        int result = waitpid(pid, &test_status, WNOHANG | WUNTRACED);

        if(result > 0) {
            printf("ok\n");
            fflush(stdout);
        }

        printf("status: %d\n", WIFSTOPPED(test_status));
        fflush(stdout);

        usleep((1 * 1000) * 3000);
        kill(pid, SIGCONT);
    }

    wait(NULL);
 */

