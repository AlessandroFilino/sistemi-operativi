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

int main(int argc, const char *argv[]) {
    srand(time(NULL));

    int pfc;
    int numberOfCharsRead;
    u_4 fallimenti; fallimenti.value = 0;
    enum boolean terminated = FALSE;

    createPipe(FILENAME_GENERATOREFALLIMENTI_PIPE, DEFAULT_PERMISSIONS);
    int fd_pipe = open(FILENAME_GENERATOREFALLIMENTI_PIPE, O_RDONLY | O_NONBLOCK);

    FILE *failures = openFile(FILENAME_FAILURES_LOG, "w");
    char buffer_newPid[PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH + 1 + 1] = {0};

    const char *pfc1_pid = argv[1];
    const char *pfc2_pid = argv[2];
    const char *pfc3_pid = argv[3];

    int pfcProcessPid[3] = {
            (int) strtol(pfc1_pid, NULL, 10),
            (int) strtol(pfc2_pid, NULL, 10),
            (int) strtol(pfc3_pid, NULL, 10)
    };

    while(!terminated) {
        usleep((1 * 1000) * 100); //100 millisecondi

        pfc = rand() % 3;
        //printf("%d, ", pfc);

        fallimenti = calcoloProb();
        //printf("\npfc: %d - value: %d\n\n", pfc+1, fallimenti.value);

        if (fallimenti.value & 1u) {
            char message[] = concat(GENERATORE_FALLIMENTI_SIGSTOP, "\n");

            //SIGSTOP = sospensione da dentro un programma
            //kill(pfcProcessPid[pfc], SIGSTOP);
            fprintf(failures, message, pfc);
        }

        if (fallimenti.value & 2u) {
            char message[] = concat(GENERATORE_FALLIMENTI_SIGINT, "\n");

            //SIGINT = quando l'utente digita ctrl-c
            //kill(pfcProcessPid[pfc], SIGINT);
            fprintf(failures, message, pfc);
        }

        if (fallimenti.value & 4u) {
            char message[] = concat(GENERATORE_FALLIMENTI_SIGCONT, "\n");

            //SIGCONT = riprende l'esecuzione di un programma dopo la sospensione
            kill(pfcProcessPid[pfc], SIGCONT);
            fprintf(failures, message, pfc);
        }

        if (fallimenti.value & 8u) {
            char message[] = concat(GENERATORE_FALLIMENTI_SIGUSR1, "\n");

            //SIGUSR1 = segnale definito dall'utente
            kill(pfcProcessPid[pfc], SIGUSR1);
            fprintf(failures, message, pfc);
        }

        numberOfCharsRead = readLine(fd_pipe, buffer_newPid, MESSAGES_SEPARATOR);
        if(numberOfCharsRead > 0) {
            removeLastChar(buffer_newPid);

            if(strcmp(buffer_newPid, APPLICATION_ENDED_MESSAGE) == 0) {
                terminated = TRUE;
            } else {
                int pfcId;
                int newFd;

                readNewFd(buffer_newPid, &pfcId, &newFd);
                pfcProcessPid[pfcId] = newFd;

                memset(buffer_newPid, '\0', sizeof(char) * strlen(buffer_newPid));
            }
        }
    }

    fclose(failures);
    close(fd_pipe);

    return 0;
}

u_4 calcoloProb() {
    u_4 prob; prob.value = 0;
    int random;
    int value;
    int inverse;

    random = rand();
    inverse = (int) inverse(PROB_SIGSTOP);
    value = random % inverse;
    if(value == 0) {
        prob.value |= 1u;
    }
    //printf("%d, ", value);

    random = rand();
    inverse = (int) inverse(PROB_SIGINT);
    value = random % inverse;
    if(value == 0) {
        prob.value |= 2u;
    }
    //printf("%d, ", value);

    random = rand();
    inverse = (int) inverse(PROB_SIGCONT);
    value = random % inverse;
    if(value == 0) {
        prob.value |= 4u;
    }
    //printf("%d, ", value);

    random = rand();
    inverse = (int) inverse(PROB_SIGUSR1);
    value = random % inverse;
    if(value == 0) {
        prob.value |= 8u;
    }
    //printf("%d, %d\n", value, prob.value);

    return prob;
}

void readNewFd(char *buffer_newPid, int *pfcId, int *newPid) {
    /*
     * TODO cambiare la lunghezza di temp_pfcId e temp_newPid
     *      considerando che pfcId può essere 1, 2 o 3 e che
     *      newPid magari potrebbe raggiungere massimo 5 cifre.
     *      (Quanto può essere lungo un pid?)
     */
    char temp_pfcId[10] = {0};
    char temp_newPid[10] = {0};

    tokenize(buffer_newPid, PFCDISCONNECTEDSWITCH_SEPARATOR, 2, temp_pfcId, temp_newPid);

    *pfcId = (int) strtol(temp_pfcId, NULL, 10);
    *newPid = (int) strtol(temp_newPid, NULL, 10);
}
