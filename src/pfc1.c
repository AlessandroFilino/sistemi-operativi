#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "utility.h"
#include "pfc.h"

enum boolean PFC1_sigusr;
void sigusrHandler(int sig);

int main(int argc, const char * argv[]) {
    double latitudine_prec = 0;
    double longitudine_prec = 0;

    signal(SIGUSR1, &sigusrHandler);

    //TODO char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";
    FILE *fp = open_file(filename_g18, "r");

    //TODO unlink va rimosso
    unlink("pfc1Pipe");
    int fd = connectPipe("pfc1Pipe", O_WRONLY);

    ssize_t read = 0;
    size_t bufferLength = MAX_LINE_LENGTH + 1;
    char *buffer = malloc(sizeof(char) * bufferLength);
    read = readCorrectLine(buffer, bufferLength, fp);
    acquisisciCoordinate(buffer, &latitudine_prec, &longitudine_prec);
    free(buffer);

    while(read != -1) {
        //TODO usare sleep(1)
        usleep((1 * 1000) * 1000); //1000 millisecondi

        read = exe(fd, fp, &latitudine_prec, &longitudine_prec, &PFC1_sigusr);
    }

    fclose(fp);
    close(fd);

    return 0;
}

void sigusrHandler(int sig) {
    changeSigusr(&PFC1_sigusr);
}






