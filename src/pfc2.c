#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h> /* For AF_UNIX sockets */
#include "../include/utility.h"
#include "../include/pfc.h"
#include "../include/path.h"

enum boolean PFC2_sigusr;
enum boolean PFC2_sigstop;
void signalHandler(int signal);

int main(int argc, const char * argv[]) {
    int last_read;
    ssize_t read = 0;
    double previousLatitude = 0;
    double previousLongitude = 0;

    signal(SIGUSR1, &signalHandler);
    signal(SIGSTOP, &signalHandler);

    //TODO char *filename_g18 = argv[1];
    char *filename_g18 = "../doc/G18.txt";
    FILE *fp_g18 = open_file(filename_g18, "r");

    last_read = open(FILENAME_LAST_READ, O_CREAT | O_RDWR);
    changePointerPosition(fp_g18, last_read);

    //TODO unlink va rimosso
    unlink (FILENAME_PFC2_SOCKET);

    int clientFd, serverLen, result;
    struct sockaddr_un serverUNIXAddress; /*Server address */
    struct sockaddr* serverSockAddrPtr; /*Ptr to server address*/
    
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    
    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; /* server domain type */
    strcpy (serverUNIXAddress.sun_path, FILENAME_PFC2_SOCKET); /* server name */

    //TODO result mi serve? o posso cambiare connectSocket() in void?
    result = connectSocket(clientFd, serverSockAddrPtr, serverLen);

    read = setPreviousGeographicCoordinates(fp_g18, &previousLatitude, &previousLongitude);

    while(read != -1) {
        //TODO usare sleep(1)
        usleep((1 * 1000) * 1000); //1000 millisecondi

        read = exe(clientFd, fp_g18, last_read, &previousLatitude, &previousLongitude, &PFC2_sigusr, &PFC2_sigstop);
    }

    fclose(fp_g18);
    close(clientFd);
}

void signalHandler(int signal) {
    setSignalStatus(signal, &PFC2_sigusr, &PFC2_sigstop);
}

