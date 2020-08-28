#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/utility.h"
#include "../include/pfc.h"
#include "../include/path.h"

enum boolean PFC3_sigusr;
enum boolean PFC3_sigstop;
void signalHandler(int signal);

int main(int argc, const char * argv[]) {
    int last_read;
    ssize_t read = 0;
    int fd_fpTransducers;
    double previousLatitude = 0;
    double previousLongitude = 0;

    signal(SIGUSR1, &signalHandler);
    signal(SIGSTOP, &signalHandler);

    //TODO char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";
    FILE *fp_g18 = open_file(filename_g18, "r");

    last_read = open(FILENAME_LAST_READ, O_CREAT | O_RDWR);
    changePointerPosition(fp_g18, last_read);

    FILE *fpTransducers = open_file(FILENAME_PFC3_FILE, "w");
    fd_fpTransducers = fileno(fpTransducers);

    read = setPreviousGeographicCoordinates(fp_g18, &previousLatitude, &previousLongitude);

    while(read != -1) {
        //TODO usare sleep(1)
        usleep((1 * 1000) * 1000); //1000 millisecondi

        read = exe(fd_fpTransducers, fp_g18, last_read, &previousLatitude, &previousLongitude, &PFC3_sigusr, &PFC3_sigstop);
        fflush(fpTransducers);
    }

    fclose(fp_g18);
    fclose(fpTransducers);
}

void signalHandler(int signal) {
    setSignalStatus(signal, &PFC3_sigusr, &PFC3_sigstop);
}