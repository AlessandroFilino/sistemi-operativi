#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include "../include/generatoreFallimenti.h"
#include "../include/utility.h"
#include "../include/path.h"
#include "../include/messages.h"

int main(int argc, const char * argv[]) {
    srand(time(NULL));

    int pfc;
    int length;
    u_4 fallimenti;
    enum boolean terminated = FALSE;
    //int signals[4] = {SIGSTOP, SIGINT, SIGCONT, SIGUSR1};

    int fd_pipe = connectPipe(FILENAME_PFCSWITCH_PIPE, O_RDONLY);
    FILE *failures = fopen(FILENAME_FAILURES_LOG, "w");
    char buffer_newPid[PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH + 1] = {0};

    const char *pfc1_pid = argv[1];
    const char *pfc2_pid = argv[2];
    const char *pfc3_pid = argv[3];

    int pfcProcessPid[3] = {
            (int) strtol(pfc1_pid, NULL, 10),
            (int) strtol(pfc2_pid, NULL, 10),
            (int) strtol(pfc3_pid, NULL, 10)
    };

    while(terminated == FALSE) {
        sleep(1);

        pfc = rand() % 3;
        fallimenti = calcoloProb();

        if (fallimenti.value & 1u) {
            /*
             * SIGSTOP = sospensione da dentro un programma
             */
            kill(pfcProcessPid[pfc], SIGSTOP);
        }

        if (fallimenti.value & 2u) {
            /*
             * SIGINT = quando l'utente digits ctrl-c
             */
            kill(pfcProcessPid[pfc], SIGINT);
        }

        if (fallimenti.value & 4u) {
            /*
             * SIGCONT = riprende l'esecuzione di un programma dopo la sospensione
             */
            kill(pfcProcessPid[pfc], SIGCONT);
        }

        if (fallimenti.value & 8u) {
            /*
             * SIGUSR1 = segnale definito dall'utente
             */
            kill(pfcProcessPid[pfc], SIGUSR1);
        }

        length = read(fd_pipe, buffer_newPid, PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH);
        if(length > 0) {
            if(strcmp(buffer_newPid, APPLICATION_ENDED_MESSAGE) == 0) {
                terminated = TRUE;
            } else {
                int pfcId;
                int newFd;

                readNewFd(buffer_newPid, &pfcId, &newFd);
                pfcProcessPid[pfcId] = newFd;
            }
        }
    }

    close(fd_pipe);

    return 0;
}

u_4 calcoloProb() {
    u_4 prob;
    int value = rand() % PROB_TOT;

    if(value < (PROB_SIGSTOP * PROB_TOT)) {
        prob.value |= 1u;
    }

    if(value < (PROB_SIGINT * PROB_TOT)) {
        prob.value |= 2u;
    }

    if(value < (PROB_SIGCONT * PROB_TOT)) {
        prob.value |= 4u;
    }

    if(value < (PROB_SIGUSR1 * PROB_TOT)) {
        prob.value |= 8u;
    }

    return prob;
}

void readNewFd(char *buffer_newPid, int *pfcId, int *newPid) {
    char temp_pfcId[10];
    char temp_newPid[10];
    char *temp[2] = {temp_pfcId, temp_newPid};

    tokenize(buffer_newPid, PFCDISCONNECTEDSWITCH_SEPARATOR, 2, temp);

    *pfcId = (int) strtol(temp[0], NULL, 10);
    *newPid = (int) strtol(temp[1], NULL, 10);

    /*
        char *temp;
        temp = strtok(buffer_newPid, PFCDISCONNECTEDSWITCH_SEPARATOR);
        *pfcId = (int) strtol(temp, NULL, 10);

        temp = strtok(NULL, PFCDISCONNECTEDSWITCH_SEPARATOR);
        *newPid = (int) strtol(temp, NULL, 10);
    */
}


