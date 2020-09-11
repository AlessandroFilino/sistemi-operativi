#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "../include/utility.h"
#include "../include/pfc.h"
#include "../include/path.h"
#include "../include/messages.h"

enum boolean PFC3_sigusr;
enum boolean PFC3_sigstop;
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

    FILE *fpTransducers = openFile(FILENAME_PFC3_FILE, "a");
    int fd_fpTransducers = fileno(fpTransducers);

    numberOfCharsRead = setPreviousGeographicCoordinates(fp_g18, &previousLatitude, &previousLongitude);
	
    while(numberOfCharsRead != -1) {
        //TODO usare sleep(1)
        usleep((1 * 1000) * 1000); //1000 millisecondi = 1 secondo

        numberOfCharsRead = exe(fd_fpTransducers, fp_g18, lastRead, &previousLatitude, &previousLongitude, &PFC3_sigusr, &PFC3_sigstop);
        fflush(fpTransducers);

	  //printf("%ld\n", numberOfCharsRead);
	  //fflush(stdout);
    }

    char message[] = concat(APPLICATION_ENDED_MESSAGE, "\n");
    int messageLength = string_length(APPLICATION_ENDED_MESSAGE) + 1;

    write(fd_fpTransducers, message, sizeof(char) * messageLength);
    //printf("%s\n", message);

    fclose(fp_g18);
    fclose(lastRead);
    fclose(fpTransducers);

    return 0;
}

void signalHandler(int signal) {
    setSignalStatus(signal, &PFC3_sigusr, &PFC3_sigstop);
}
