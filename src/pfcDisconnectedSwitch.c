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
#include "../include/messages.h"

int main(int argc, const char *argv[]) {
    //char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";

    int read;
    int pid_pfc;
    int pfcNumber;
    int pfcProcessPid[3];
    int generatoreFallimentiProcess;
    enum boolean terminated = FALSE;
    char error[WES_MESSAGE_MAX_LENGTH] = {0};

    createPipe(FILENAME_WES_PIPE);
    createPipe(FILENAME_PFCSWITCH_PIPE);

    int PfcSwitchPipe = open(FILENAME_PFCSWITCH_PIPE, O_WRONLY);
    int wesPipe = open(FILENAME_WES_PIPE, O_RDONLY);
    FILE *switchLog = openFile(FILENAME_SWITCH_LOG, "w");

    char *PFC1_argv[] = {"pfc1", filename_g18, NULL};
    pfcProcessPid[0] = createChild(&execv, "pfc1", PFC1_argv);

    char *PFC2_argv[] = {"pfc2", filename_g18, NULL};
    pfcProcessPid[1] = createChild(&execv, "pfc2", PFC2_argv);

    char *PFC3_argv[] = {"pfc3", filename_g18, NULL};
    pfcProcessPid[2] = createChild(&execv, "pfc3", PFC3_argv);

    /*
     * alloco 3 buffer per contenere i PID dei processi PFC
     * in forma di stringa. snprintf mi permette appunto di
     * scrivere ogni PID all'interno del corrispondente buffer
     * sottoforma di stringa. I PID devono essere inviati come
     * parametri al processo generatoreFallimenti.
     */
    char pfc1Process[10] = {0};
    char pfc2Process[10] = {0};
    char pfc3Process[10] = {0};
    snprintf(pfc1Process, sizeof(char) * 10, "%d", pfcProcessPid[0]);
    snprintf(pfc2Process, sizeof(char) * 10, "%d", pfcProcessPid[1]);
    snprintf(pfc3Process, sizeof(char) * 10, "%d", pfcProcessPid[2]);

    char *generatoreFallimenti_argv[] = {"generatoreFallimenti", pfc1Process, pfc2Process, pfc3Process};
    generatoreFallimentiProcess = createChild(&execv, "generatoreFallimenti", generatoreFallimenti_argv);

    /*
     * TODO: usare una macro al posto di **pfcArgv[] per creare i token
     *       #define get_argv(pfcId) PFC##pfcId##_argv
     */
    char **pfcArgv[] = {PFC1_argv, PFC2_argv, PFC3_argv};

    while(!terminated) {
        read = readLine(wesPipe, error, '\0');

        /*
         * se la pipe è vuota, readLine restituisce 0
         * se avviene un errore durante la lettura, readLine restituisce -1
         */
        if(read > 0) {
            if(strcmp(error, APPLICATION_ENDED_MESSAGE) == 0) {
                terminated = TRUE;
                //TODO inviare "Terminated" a generatoreFallimenti

                /* Non abbiamo usato signal(SIGCHLD, SIG_IGN); per ignorare la morte
                 * dei processi figli per prevenire zombie perchè ciò crea figli orfani.
                 * Abbiamo preferito usare la wait ripetuta 4 volte (pfc1/2/3 e generatoreFallimenti)
                 */
                for(int i=0; i<4; i++) {
                    wait(NULL);
                }
            } else if(strcmp(error, WES_MESSAGE_EMERGENCY) == 0) {
                kill(pfcProcessPid[0], SIGUSR2);
                kill(pfcProcessPid[1], SIGUSR2);
                kill(pfcProcessPid[2], SIGUSR2);

                fprintf(switchLog, "SIGUSR2 sent to PFC1, PFC2, PFC3\n");
            } else {
                pfcNumber = getErrorInfo(error);
                pid_pfc = pfcProcessPid[pfcNumber - 1];

                /*
                 * kill(pidFiglio, 0) restituisce 0 se esiste un processo
                 * con pid_pfc uguale a pidFiglio, altrimenti -1
                 */
                if(kill(pid_pfc, 0) == 0) {
                    //il processo esiste

                    int status = 0;
                    waitpid(pid_pfc, &status, WNOHANG | WUNTRACED);

                    if(WIFSTOPPED(status) != 0) {
                        //il processo è bloccato

                        kill(pid_pfc, SIGCONT);
                        fprintf(switchLog, "SIGCONT sent to PFC%d\n", pfcNumber);
                    }
                } else {
                    //il processo non esiste più

                    char filename[4];
                    snprintf(filename, sizeof(char) * 4, "pfc%d", pfcNumber);

                    int newPid = createChild(&execv, filename, pfcArgv[pfcNumber - 1]);
                    pfcProcessPid[pfcNumber - 1] = newPid;
                    fprintf(switchLog, "PFC%d è stato ricreato\n", pfcNumber);

                    char message[PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH];
                    snprintf(message, sizeof(char) * PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH, "%d%c%d", pfcNumber, PFCDISCONNECTEDSWITCH_SEPARATOR, newPid);
                    write(PfcSwitchPipe, message, PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH);
                }
            }
        }
    }

    close(wesPipe);

    return 0;
}

/*
    void childHandler(int sig) {
        int childPid, childStatus;


        childPid = wait(&childStatus);
        printf ("Child %d terminated\n", childPid);

        exit(EXIT_SUCCESS);
    }
 */

int getErrorInfo(char *error) {
    unsigned int size = (unsigned int) string_length(error);

    int result = 0;
    char *endPointer = &error[size-1];

    while(*endPointer >= '0' && *endPointer <= '9') {
        result = result * 10 + (*endPointer - '0');
        endPointer--;
    }

    return result;
}

