#include "transducers.h"

int main(int argc, const char * argv[]) {
    int tempo = 0; //millisecondi trascorsi


    //pfc1
    char velocita_pfc1[50];
    unlink("pfc1Pipe");
    mknod("pfc1Pipe", S_IFIFO, 0);
    chmod("pfc1Pipe", 0660);
    int fd = open("pfc1Pipe", O_RDONLY | O_NONBLOCK);
    FILE *speedPFC1Log = file_open("speedPFC1.log", "w");

    //pfc2
    int serverFd, clientFd, serverLen, result;
    int unsigned clientLen;
    struct sockaddr_un serverUNIXAddress; /*Server address */
    struct sockaddr* serverSockAddrPtr; /*Ptr to server address*/
    struct sockaddr_un clientUNIXAddress; /*Client address */
    struct sockaddr* clientSockAddrPtr;/*Ptr to client address*/

    signal(SIGCHLD, SIG_IGN); /*ignore death-of-child signals to prevent zombies */

    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; /* Set domain type */
    strcpy (serverUNIXAddress.sun_path, "pfc2"); /* Set name */
    unlink ("pfc2"); /* Remove file if it already exists */
    bind (serverFd, serverSockAddrPtr, serverLen);/*Create file*/
    listen (serverFd, 1); /* Maximum pending connection length */

    FILE *speedPFC2Log = fopen("speedPFC2.log", "w");
    char* velocita_pfc2[50];
    long caratteriLetti;
    size_t lunghezzaRiga = 5;


    //pfc3
    FILE *speedPFC3Log = fopen("speedPFC3.log", "w");
    FILE *fpTransducers = fopen("pfcTransducers.txt", "r");
    int velocita_pfc3;


    //pfc2
    if (fork () == 0) { /* Create child to send receipe */
        for(;;) {
            usleep(5);

            clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
            caratteriLetti = getline(&velocita_pfc2, &lunghezzaRiga, serverFd);
        }
    }


    for(;;) {
        usleep(5);

        //pfc1
        read(fd, velocita_pfc1, sizeof(int));

        //pfc3
        fread(&velocita_pfc3, sizeof(int), 1, fpTransducers);


        if(tempo == 200) {
            fflush(speedPFC1Log);
            fflush(speedPFC2Log);
            fflush(speedPFC3Log);

            fwrite(velocita_pfc1, sizeof(int), 1, speedPFC1Log);
            fwrite(&velocita_pfc2, sizeof(int), 1, speedPFC2Log);
            fwrite(&velocita_pfc3, sizeof(int), 1, speedPFC3Log);

            velocita_pfc1 = -1;
            velocita_pfc2 = -1;
            velocita_pfc3 = -1;
        }

        tempo++;
    }

    close (clientFd); /* Close the socket */

    return 0;
}

