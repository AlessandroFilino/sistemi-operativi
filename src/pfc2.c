#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/utility-ipc.h"
#include "../include/pfc.h"
#include "../include/config.h"
#include "../include/messages.h"

enum boolean PFC2_sigUsr;
enum boolean PFC2_sigRestart;
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

    int transducersPipe = connectPipe(FILENAME_PFC2_PIPE, O_WRONLY);
    numberOfCharsRead = setPreviousGeographicCoordinates(fp_g18, &previousLatitude, &previousLongitude);

    while(numberOfCharsRead != -1) {
        sleep(1);
        numberOfCharsRead = exe(transducersPipe, fp_g18, lastRead, &previousLatitude, &previousLongitude, &PFC2_sigUsr, &PFC2_sigRestart);
    }

    char message[] = concat(APPLICATION_ENDED_MESSAGE, "\n");
    int messageLength = string_length(APPLICATION_ENDED_MESSAGE) + 1;

    write(transducersPipe, message, sizeof(char) * messageLength);


    fclose(fp_g18);
    fclose(lastRead);
    close(transducersPipe);

    return 0;
}

void signalHandler(int signal) {
    setSignalStatus(signal, &PFC2_sigUsr, &PFC2_sigRestart);
}






