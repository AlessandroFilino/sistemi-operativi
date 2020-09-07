#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "../include/utility.h"
#include "../include/pfc.h"
#include "../include/path.h"
#include "../include/messages.h"

enum boolean PFC2_sigusr;
enum boolean PFC2_sigstop;
void signalHandler(int signal);

int main(int argc, const char * argv[]) {
    ssize_t numberOfCharsRead = 0;
    double previousLatitude = 0;
    double previousLongitude = 0;

    signal(SIGUSR1, &signalHandler);
    signal(SIGSTOP, &signalHandler);

    //TODO char *filename_g18 = argv[1];
    char *filename_g18 = "../doc/G18.txt";
    FILE *fp_g18 = openFile(filename_g18, "r");

    FILE *lastRead = openFile(FILENAME_LAST_READ, "r+");
    changePointerPosition(fp_g18, lastRead);

    //TODO unlink va rimosso
    //unlink(FILENAME_PFC2_SOCKET);

    int clientFd;
    unsigned int serverLen;
    struct sockaddr_un serverUNIXAddress; //Server address
    struct sockaddr* serverSockAddrPtr; //Ptr to server address
    clientFd = createClientAF_UNIXSocket(FILENAME_PFC2_SOCKET, &serverUNIXAddress, &serverSockAddrPtr, &serverLen);
    connectSocket(clientFd, serverSockAddrPtr, serverLen);

    numberOfCharsRead = setPreviousGeographicCoordinates(fp_g18, &previousLatitude, &previousLongitude);

    while(numberOfCharsRead != -1) {
        //TODO usare sleep(1)
        usleep((1 * 1000) * 100); //1000 millisecondi = 1 secondo

        numberOfCharsRead = exe(clientFd, fp_g18, lastRead, &previousLatitude, &previousLongitude, &PFC2_sigusr, &PFC2_sigstop);
    }

    char message[] = concat(APPLICATION_ENDED_MESSAGE, "\n");
    int messageLength = string_length(APPLICATION_ENDED_MESSAGE) + 1;

    write(clientFd, message, sizeof(char) * messageLength);
    printf("%s\n", message);

    fclose(fp_g18);
    fclose(lastRead);
    close(clientFd);

    return 0;
}

void signalHandler(int signal) {
    setSignalStatus(signal, &PFC2_sigusr, &PFC2_sigstop);
}

