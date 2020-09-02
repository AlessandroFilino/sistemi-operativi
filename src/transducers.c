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
    int read = 0;

    //pfc1
    char *velocita_pfc1 = calloc(10, sizeof(char));
    createPipe(FILENAME_PFC1_PIPE);
    int fd_PFC1pipe = open(FILENAME_PFC1_PIPE, O_RDONLY | O_NONBLOCK);
    FILE *speedPFC1Log = openFile(FILENAME_SPEEDPFC1_LOG, "w");

    //pfc2
    int serverFd, clientFd;
    unsigned int clientLen;
    struct sockaddr* clientSockAddrPtr;//Ptr to client address

    serverFd = createServerAF_UNIXSocket(FILENAME_PFC2_SOCKET, MAXIMUM_CONNECTIONS, &clientSockAddrPtr, &clientLen);
    clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
    setFileFlags(clientFd, O_NONBLOCK);

    FILE *speedPFC2Log = openFile(FILENAME_SPEEDPFC2_LOG, "w");
    char* velocita_pfc2 = calloc(10, sizeof(char));

    //pfc3
    int fd_PFC3File = open(FILENAME_PFC3_FILE, O_RDONLY);
    FILE *speedPFC3Log = openFile(FILENAME_SPEEDPFC3_LOG, "w");
    char *velocita_pfc3 = calloc(10, sizeof(char));

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
        read = readLine(fd_PFC1pipe, velocita_pfc1, '\0');
        if(read > 0) {
            memset(velocita_pfc1, '\0', sizeof(char) * 10);
            fwrite(velocita_pfc1, sizeof(char), read, speedPFC1Log);
            fflush(speedPFC1Log);
            printf("pfc1 - transducer: %s (%d)\n", velocita_pfc1, tempo);
        }
        printf("pfc1 - transducer: %s (%d)\n", velocita_pfc1, tempo);


        //pfc2
        read = readLine(clientFd, velocita_pfc2, '\0');
        if(read > 0) {
            memset(velocita_pfc2, '\0', sizeof(char) * 10);
            fwrite(velocita_pfc2, sizeof(char), read, speedPFC2Log);
            fflush(speedPFC2Log);
            printf("pfc2 - transducer: %s (%d)\n", velocita_pfc2, tempo);
        }
        printf("pfc2 - transducer: %s (%d)\n", velocita_pfc2, tempo);


        //pfc3
        read = readLine(fd_PFC3File, velocita_pfc3, '\0');
        if(read > 0) {
            memset(velocita_pfc3, '\0', sizeof(char) * 10);
            fwrite(velocita_pfc3, sizeof(char), read, speedPFC3Log);
            fflush(speedPFC3Log);
            printf("pfc3 - transducer: %s (%d)\n", velocita_pfc3, tempo);
        }
        printf("pfc3 - transducer: %s (%d)\n", velocita_pfc3, tempo);

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

