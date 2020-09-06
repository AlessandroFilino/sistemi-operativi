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
    struct sockaddr* clientSockAddrPtr;

    serverFd = createServerAF_UNIXSocket(FILENAME_PFC2_SOCKET, MAXIMUM_CONNECTIONS, &clientSockAddrPtr, &clientLen);
    clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
    setFileFlags(clientFd, O_NONBLOCK);

    FILE *speedPFC2Log = openFile(FILENAME_SPEEDPFC2_LOG, "w");
    char velocita_pfc2[10] = {0};

    //pfc3
    int fd_PFC3File = open(FILENAME_PFC3_FILE, O_RDONLY | O_CREAT, 0660);
    FILE *speedPFC3Log = openFile(FILENAME_SPEEDPFC3_LOG, "w");
    char velocita_pfc3[10] = {0};

    /*
     * I messaggi ricevuti da pfc1/2/3 contengono un carattere '\n'
     * che permette di distinguere un messaggio dall'altro.
     * Dovendo scrivere i valori delle velocità nei file di log,
     * è opportuno utilizzare anche in questo caso un carattere
     * che separi tali valori per rendere i file di log leggibili.
     * Abbiamo deciso di usare il medesimo carattere, ovvero MESSAGES_SEPARATOR.
     * Successivamente, il while deve controllare se velocita_pfc1/2/3 contengono
     * la stringa di terminazione, perciò è necessario rimuovere il carattere
     * MESSAGES_SEPARATOR
     */

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
         *
         * TODO: però forse leggendo da un buffer vuoto, velocita_pfc1,
         *       velocita_pfc2, velocita_pfc3 vengono sporcate con valori
         *       rimasti in memoria? Se si, ha senso lasciare il memset fuori
         *       dall'if, altrimenti va inserito dentro l'if
         */

        //pfc1
        numberOfCharsRead = readLine(fd_PFC1pipe, velocita_pfc1, MESSAGES_SEPARATOR);
        //printf("pfc1 - transducer: %s (%d)\n", velocita_pfc1, tempo);
        if(numberOfCharsRead > 0) {
            printf("pfc1 - transducer: (%d) %s", tempo, velocita_pfc1);
            fflush(stdout);

            fprintf(speedPFC1Log, "%s", velocita_pfc1);
            fflush(speedPFC1Log);

            removeLastChar(velocita_pfc1);
        }
        memset(velocita_pfc1, '\0', sizeof(char) * 10);

        //pfc2
        numberOfCharsRead = readLine(clientFd, velocita_pfc2, MESSAGES_SEPARATOR);
        //printf("pfc2 - transducer: %s (%d)\n", velocita_pfc2, tempo);
        if(numberOfCharsRead > 0) {
            printf("pfc2 - transducer: (%d) %s", tempo, velocita_pfc2);
            fflush(stdout);

            fprintf(speedPFC2Log, "%s", velocita_pfc2);
            fflush(speedPFC2Log);

            removeLastChar(velocita_pfc2);
        }
        memset(velocita_pfc2, '\0', sizeof(char) * 10);

        //pfc3
        numberOfCharsRead = readLine(fd_PFC3File, velocita_pfc3, MESSAGES_SEPARATOR);
        //printf("pfc3 - transducer: %s (%d)\n", velocita_pfc3, tempo);
        if(numberOfCharsRead > 0) {
            printf("pfc3 - transducer: (%d) %s\n", tempo, velocita_pfc3);
            fflush(stdout);

            fprintf(speedPFC3Log, "%s", velocita_pfc3);
            fflush(speedPFC3Log);

            removeLastChar(velocita_pfc3);
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

