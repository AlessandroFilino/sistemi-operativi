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

int main(int argc, const char * argv[]) {
    int tempo = 0; //microsecondi trascorsi
    int read = 0;

    //pfc1
    char *velocita_pfc1 = calloc(10, sizeof(char));
    unlink("pfc1Pipe");
    mknod("pfc1Pipe", S_IFIFO, 0);
    chmod("pfc1Pipe", 0660);
    int fd_PFC1 = open("pfc1Pipe", O_RDONLY | O_NONBLOCK);
    FILE *speedPFC1Log = open_file("../sistemioperativi/log/speedPFC1.log", "w");

    //pfc2
    int serverFd, clientFd, serverLen, result;
    int unsigned clientLen;
    struct sockaddr_un serverUNIXAddress; //Server address
    struct sockaddr* serverSockAddrPtr; //Ptr to server address
    struct sockaddr_un clientUNIXAddress; //Client address
    struct sockaddr* clientSockAddrPtr;//Ptr to client address

    //TODO mi serve questa signal?
    signal(SIGCHLD, SIG_IGN); //ignore death-of-child signals to prevent zombies

    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; // Set domain type
    strcpy (serverUNIXAddress.sun_path, "transducers_server"); // Set name
    unlink ("transducers_server"); // Remove file if it already exists
    bind (serverFd, serverSockAddrPtr, serverLen);// Create file
    listen (serverFd, 1); // Maximum pending connection length
    clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);

    // TODO: recv(clientFd, velocita_pfc2, sizeof(char) * 6, MSG_DONTWAIT) oppure fcntl(clientFd, F_SETFL, O_NONBLOCK)?
    int flags = fcntl(clientFd, F_GETFL);
    if(flags == -1) {
        exit(EXIT_FAILURE);
    }

    if(fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) != 0) {
        exit(EXIT_FAILURE);
    }

    FILE *speedPFC2Log = open_file("../sistemioperativi/log/speedPFC2.log", "w");
    char* velocita_pfc2 = calloc(10, sizeof(char));

    //pfc3
    FILE *speedPFC3Log = open_file("../sistemioperativi/log/speedPFC3.log", "w");
    int fd_PFC3 = open("pfcTransducers.txt", O_RDONLY);
    char *velocita_pfc3 = calloc(10, sizeof(char));

    /*usleep(1000);

    velocita_pfc2 = calloc(6, sizeof(char));
    read(clientFd, velocita_pfc2, sizeof(char) * 6);
    printf("-- transducer: %s\n", velocita_pfc2);
    free(velocita_pfc2);

    velocita_pfc2 = calloc(6, sizeof(char));
    read(clientFd, velocita_pfc2, sizeof(char) * 6);
    printf("-- transducer: %s\n", velocita_pfc2);
    free(velocita_pfc2);*/

    for(;;) {
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
        read = readLine(fd_PFC1, velocita_pfc1, '\0');
        if(velocita_pfc1[0] != '\0') {
            fwrite(velocita_pfc1, sizeof(char), read, speedPFC1Log);
            fflush(speedPFC1Log);
            printf("pfc1 - transducer: %s (%d)\n", velocita_pfc1, tempo);
            memset(velocita_pfc1, '\0', sizeof(char) * 10);
        }
        printf("pfc1 - transducer: %s (%d)\n", velocita_pfc1, tempo);


        //pfc2
        read = readLine(clientFd, velocita_pfc2, '\0');
        if(velocita_pfc2[0] != '\0') {
            fwrite(velocita_pfc2, sizeof(char), read, speedPFC2Log);
            fflush(speedPFC2Log);
            printf("pfc2 - transducer: %s (%d)\n", velocita_pfc2, tempo);
            memset(velocita_pfc2, '\0', sizeof(char) * 10);
        }
        printf("pfc2 - transducer: %s (%d)\n", velocita_pfc2, tempo);


        //pfc3
        read = readLine(fd_PFC3, velocita_pfc3, '\0');
        if(velocita_pfc3[0] != '\0') {
            fwrite(velocita_pfc3, sizeof(char), read, speedPFC3Log);
            fflush(speedPFC3Log);
            printf("pfc3 - transducer: %s (%d)\n", velocita_pfc3, tempo);
            memset(velocita_pfc3, '\0', sizeof(char) * 10);
        }
        printf("pfc3 - transducer: %s (%d)\n", velocita_pfc3, tempo);


        if(strncmp(velocita_pfc1, "-1", 2) == 0) {
            break;
        }

        /*if(tempo == 20) {
            break;
        }*/

        tempo++;
    }

    //pfc1
    close(fd_PFC1);
    fclose(speedPFC1Log);

    //pfc2
    close(clientFd); // Close the socket
    fclose(speedPFC2Log);

    //pfc3
    close(fd_PFC3);
    fclose(speedPFC3Log);

    return 0;
}

