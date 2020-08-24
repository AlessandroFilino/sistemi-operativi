#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/utility.h"
#include "../include/pfc.h"
#include "../include/path.h"

enum boolean PFC3_sigusr;
void sigusrHandler(int sig);

int main(int argc, const char * argv[]) {
    double latitudine_prec = 0;
    double longitudine_prec = 0;

    signal(SIGUSR1, &sigusrHandler);

    //TODO char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";
    FILE *fp_g18 = open_file(filename_g18, "r");

    char *filename_last_read = FILENAME_LAST_READ;
    FILE *last_read = open_file(filename_last_read, "a+");

    char pathname_pfcTransducers[50] = {0};
    strcpy(pathname_pfcTransducers, PATHNAME_TEMP);
    strcat(pathname_pfcTransducers, "pfcTransducers.txt");
    FILE *fpTransducers = open_file(pathname_pfcTransducers, "w");
    int fd = fileno(fpTransducers);

    ssize_t read = 0;
    size_t bufferLength = MAX_LINE_LENGTH + 1;
    char *buffer = malloc(sizeof(char) * bufferLength);
    read = readCorrectLine(buffer, bufferLength, fp_g18);
    acquisisciCoordinate(buffer, &latitudine_prec, &longitudine_prec);
    free(buffer);

    while(read != -1) {
        //TODO usare sleep(1)
        usleep((1 * 1000) * 1000); //1000 millisecondi

        read = exe(fd, fp_g18, last_read, &latitudine_prec, &longitudine_prec, &PFC3_sigusr);
        fflush(fpTransducers);
    }

    fclose(fp_g18);
    fclose(fpTransducers);
}

void sigusrHandler(int sig) {
    changeSigusr(&PFC3_sigusr);
}