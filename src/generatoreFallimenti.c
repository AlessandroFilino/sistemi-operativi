#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include "../include/generatoreFallimenti.h"
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/utility-ipc.h"
#include "../include/config.h"
#include "../include/messages.h"

int main(int argc, const char *argv[]) {

    srand(time(NULL));

    int pfc;
    int pfcId;
    int newFd;
    int numberOfCharsRead;
    unsigned char fallimenti = 0;
    enum boolean terminated = FALSE;

    createPipe(FILENAME_GENERATOREFALLIMENTI_PIPE, DEFAULT_PERMISSIONS);
    int fd_pipe = open(FILENAME_GENERATOREFALLIMENTI_PIPE, O_RDONLY | O_NONBLOCK);

    FILE *failures = openFile(FILENAME_FAILURES_LOG, "w");
    char buffer_newPid[PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH] = {0};

    int pfcProcessPid[3] = {
            (int) strtol(argv[1], NULL, 10),
            (int) strtol(argv[2], NULL, 10),
            (int) strtol(argv[3], NULL, 10)
    };

    while(!terminated) {
        sleep(1);

        pfc = rand() % 3;
        fallimenti = setProb();

        if (fallimenti & SIGSTOP_INDEX) {
            //SIGSTOP = sospensione da dentro un programma
            kill(pfcProcessPid[pfc], SIGSTOP);

            writeLog(failures, GENERATOREFALLIMENTI_SIGSTOP, pfc+1);
        }

        if (fallimenti & SIGINT_INDEX) {
            //SIGINT = quando l'utente digita ctrl-c
            kill(pfcProcessPid[pfc], SIGINT);

            writeLog(failures, GENERATOREFALLIMENTI_SIGINT, pfc+1);
        }

        if (fallimenti & SIGCONT_INDEX) {
            //SIGCONT = riprende l'esecuzione di un programma dopo la sospensione
		    kill(pfcProcessPid[pfc], SIGCONT);

            writeLog(failures, GENERATOREFALLIMENTI_SIGCONT, pfc+1);
        }

        if (fallimenti & SIGUSR1_INDEX) {
            //SIGUSR1 = segnale definito dall'utente
            kill(pfcProcessPid[pfc], SIGUSR1);

            writeLog(failures, GENERATOREFALLIMENTI_SIGUSR1, pfc+1);
        }

        numberOfCharsRead = readLine(fd_pipe, buffer_newPid, MESSAGES_SEPARATOR);
        if(numberOfCharsRead > 0) {
            removeLastChar(buffer_newPid);

            if(strcmp(buffer_newPid, APPLICATION_ENDED_MESSAGE) == 0) {
                terminated = TRUE;
            } else {
			    printf("%s\n", buffer_newPid);
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

void writeLog(FILE *log, char *string, int pfc) {
    char message[GENERATOREFALLIMENTI_MESSAGE_MAX_LENGTH] = {0};
    strcpy(message, string);
    message[strlen(message)] = '\n';

    fprintf(log, message, pfc);
    fflush(log);
}

enum boolean checkProb(double prob) {
    int random = rand();
    int inv = (int) inverse(prob);
    int value = random % inv;

    return (value == 0);
}

unsigned char setProb() {
    unsigned char prob = 0;

    if(checkProb(PROB_SIGSTOP)) {
        prob |= SIGSTOP_INDEX;
    }

    if(checkProb(PROB_SIGINT)) {
        prob |= SIGINT_INDEX;
    }

    if(checkProb(PROB_SIGCONT)) {
        prob |= SIGCONT_INDEX;
    }

    if(checkProb(PROB_SIGUSR1)) {
        prob |= SIGUSR1_INDEX;
    }

    return prob;
}

void readNewFd(char *buffer_newPid, int *pfcId, int *newPid) {
    char temp_pfcId[PFC_ID_MAX_DIGITS] = {0};
    char temp_newPid[PID_MAX_LENGTH] = {0};

    tokenize(buffer_newPid, PFCDISCONNECTEDSWITCH_SEPARATOR, 2, temp_pfcId, temp_newPid);

    *pfcId = (int) strtol(temp_pfcId, NULL, 10);
    *newPid = (int) strtol(temp_newPid, NULL, 10);
}
