#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include "../include/utility.h"
#include "../include/pfc.h"
#include "../include/path.h"

enum boolean PFC1_sigusr;
void sigusrHandler(int sig);

int main(int argc, const char * argv[]) {
    double latitudine_prec = 0;
    double longitudine_prec = 0;

    signal(SIGUSR1, &sigusrHandler);
    signal(SIGCONT, &sigusrHandler);

    //TODO char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";
    FILE *fp_g18 = open_file(filename_g18, "r");

    char *filename_last_read = {0};
    strcpy(filename_last_read, PATHNAME_TEMP);
    strcat(filename_last_read, "last_read.txt");
    int last_read = open(filename_last_read, O_CREAT | O_RDWR);

    /* buffer_position è un buffer che contiene la posizione letta da last_read sottoforma di stringa */
    char buffer_position[MAX_G18_FILE_LENGTH_DIGITS] = {0};
    long position = 0;
    int bytes = read(last_read, buffer_position, MAX_G18_FILE_LENGTH_DIGITS);
    if(bytes != 0) {
        position = strtol(buffer_position, NULL, 10);
        fseek(fp_g18, position, SEEK_SET);
    }

    //TODO unlink va rimosso
    unlink("pfc1Pipe");
    char pathname_pipe[50] = {0};
    strcpy(pathname_pipe, PATHNAME_TEMP);
    strcat(pathname_pipe, "pfc1Pipe");
    int fd_pipe = connectPipe(pathname_pipe, O_WRONLY);

    ssize_t read = 0;
    size_t bufferLength = MAX_LINE_LENGTH + 1;
    char *buffer = malloc(sizeof(char) * bufferLength);
    read = readCorrectLine(buffer, bufferLength, fp_g18);
    acquisisciCoordinate(buffer, &latitudine_prec, &longitudine_prec);
    free(buffer);

    while(read != -1) {
        //TODO usare sleep(1)
        usleep((1 * 1000) * 1000); //1000 millisecondi

        read = exe(fd_pipe, fp_g18, last_read, &latitudine_prec, &longitudine_prec, &PFC1_sigusr);
    }

    fclose(fp_g18);
    close(fd_pipe);

    return 0;
}

void sigusrHandler(int sig) {
    switch(sig) {
        case SIGUSR1: {
            changeSigusr(&PFC1_sigusr);
            break;
        }

        case SIGCONT: {
            //TODO leggere la posizione da last_read.txt e leggere da g18.txt la riga corrispondent per settare latitudine_prec e longitudine_prec
            break;
        }
    }
}






