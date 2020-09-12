#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/utility-ipc.h"
#include "../include/pfc.h"
#include "../include/config.h"
#include "../include/messages.h"

enum boolean PFC2_sigusr;
enum boolean PFC2_sigstop;
void signalHandler(int signal);

int main(int argc, const char * argv[]) {
    ssize_t numberOfCharsRead = 0;
    double previousLatitude = 0;
    double previousLongitude = 0;

    signal(SIGUSR1, &signalHandler);
    signal(SIGCONT, &signalHandler);

    const char *filename_g18 = argv[1];
    FILE *fp_g18 = openFile(filename_g18, "r");

    FILE *lastRead = openFile(FILENAME_LAST_READ, "r+");
    changePointerPosition(fp_g18, lastRead);

    int clientFd;
    unsigned int serverLen;
    struct sockaddr_un serverUNIXAddress; //Server address
    struct sockaddr* serverSockAddrPtr; //Ptr to server address
    clientFd = createClientAF_UNIXSocket(FILENAME_PFC2_SOCKET, &serverUNIXAddress, &serverSockAddrPtr, &serverLen);
    connectSocket(clientFd, serverSockAddrPtr, serverLen);

    numberOfCharsRead = setPreviousGeographicCoordinates(fp_g18, &previousLatitude, &previousLongitude);

    while(numberOfCharsRead != -1) {
        //TODO usare sleep(1)
        usleep((1 * 1000) * 1000); //1000 millisecondi = 1 secondo

	  /*fseek(lastRead, 0, SEEK_SET);
	  fread(test, sizeof(char), 7, lastRead);
	  fseek(lastRead, 0, SEEK_SET);
	  printf("pfc2: %s\n", test);
	  fflush(stdout);*/

        numberOfCharsRead = exe(clientFd, fp_g18, lastRead, &previousLatitude, &previousLongitude, &PFC2_sigusr, &PFC2_sigstop);
    }

    char message[] = concat(APPLICATION_ENDED_MESSAGE, "\n");
    int messageLength = string_length(APPLICATION_ENDED_MESSAGE) + 1;

    write(clientFd, message, sizeof(char) * messageLength);
    //sprintf("%s\n", message);

    fclose(fp_g18);
    fclose(lastRead);
    close(clientFd);

    return 0;
}

void signalHandler(int signal) {
    setSignalStatus(signal, &PFC2_sigusr, &PFC2_sigstop);
}

