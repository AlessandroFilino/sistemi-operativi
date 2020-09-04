#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> /* For AF_UNIX sockets */
#include "../include/utility.h"
#include "../include/transducers.h"
#include "../include/path.h"
#include "../include/messages.h"

int main(int argc, const char * argv[]) {
    int tempo = 0; //microsecondi trascorsi
    int numberOfCharsRead = 0;

    //pfc1
    createPipe(FILENAME_PFC1_PIPE);
    int fd_PFC1pipe = open(FILENAME_PFC1_PIPE, O_RDONLY | O_NONBLOCK);
    FILE *speedPFC1Log = openFile(FILENAME_SPEEDPFC1_LOG, "w");
    char velocita_pfc1[10] = {0};

    //pfc2
    int serverFd, clientFd;
    unsigned int clientLen;
    struct sockaddr* clientSockAddrPtr;//Ptr to client address

    serverFd = createServerAF_UNIXSocket(FILENAME_PFC2_SOCKET, MAXIMUM_CONNECTIONS, &clientSockAddrPtr, &clientLen);
    clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
    setFileFlags(clientFd, O_NONBLOCK);

    FILE *speedPFC2Log = openFile(FILENAME_SPEEDPFC2_LOG, "w");
    char velocita_pfc2[10] = {0};

    //pfc3
    int fd_PFC3File = open(FILENAME_PFC3_FILE, O_RDONLY | O_CREAT, 0660);
    FILE *speedPFC3Log = openFile(FILENAME_SPEEDPFC3_LOG, "w");
    char velocita_pfc3[10] = {0};

    while((strcmp(velocita_pfc1, APPLICATION_ENDED_MESSAGE) != 0) ||
            (strcmp(velocita_pfc2, APPLICATION_ENDED_MESSAGE) != 0) ||
            (strcmp(velocita_pfc3, APPLICATION_ENDED_MESSAGE) != 0)) {

        //TODO usare usleep(5000) --> sleep di 5 millisecondi
        usleep((1 * 1000) * 100); //100 millisecondi

        /*
         * TODO: Visto che il transducers va più veloce
         *       dei pfc, non c'è bisogno di settare a zero
         *       le stringhe velocita_pfc1, velocita_pfc2,
         *       velocita_pfc3 perchè, leggendo da un buffer vuoto,
         *       si setteranno a zero da sole (?)
         */

        //pfc1
        numberOfCharsRead = readLine(fd_PFC1pipe, velocita_pfc1, '\n');
        //printf("pfc1 - transducer: %s (%d)\n", velocita_pfc1, tempo);
        if(numberOfCharsRead > 0) {
            printf("pfc1 - transducer: %s (%d)\n", velocita_pfc1, tempo);
            fflush(stdout);

            //TODO rimuovere '/n' dalla stringa letta da fd_PFC1pipe
            fprintf(speedPFC1Log, "%s\n", velocita_pfc1);
            fflush(speedPFC1Log);
        }
        memset(velocita_pfc1, '\0', sizeof(char) * 10);

        //pfc2
        numberOfCharsRead = readLine(clientFd, velocita_pfc2, '\n');
        //printf("pfc2 - transducer: %s (%d)\n", velocita_pfc2, tempo);
        if(numberOfCharsRead > 0) {
            printf("pfc2 - transducer: %s (%d)\n", velocita_pfc2, tempo);
            fflush(stdout);

            //TODO rimuovere '/n' dalla stringa letta da clientFd
            fprintf(speedPFC2Log, "%s\n", velocita_pfc2);
            fflush(speedPFC2Log);
        }
        memset(velocita_pfc2, '\0', sizeof(char) * 10);

        //pfc3
        numberOfCharsRead = readLine(fd_PFC3File, velocita_pfc3, '\n');
        //printf("pfc3 - transducer: %s (%d)\n", velocita_pfc3, tempo);
        if(numberOfCharsRead > 0) {
            printf("pfc3 - transducer: %s (%d)\n", velocita_pfc3, tempo);
            fflush(stdout);

            //TODO rimuovere '/n' dalla stringa letta da fd_PFC3File
            fprintf(speedPFC3Log, "%s\n", velocita_pfc3);
            fflush(speedPFC3Log);
        }
        memset(velocita_pfc3, '\0', sizeof(char) * 10);


        /*if(tempo == 20) {
            break;
        }*/

        tempo++;
    }

    //pfc1
    close(fd_PFC1pipe);
    fclose(speedPFC1Log);

    //pfc2
    close(clientFd);
    close(serverFd);
    fclose(speedPFC2Log);

    //pfc3
    close(fd_PFC3File);
    fclose(speedPFC3Log);

    return 0;
}

