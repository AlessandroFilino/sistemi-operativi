#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/wes.h"
#include "../include/utility.h"
#include "../include/path.h"

int main(int argc, const char *argv[]) {
    int speedPFC1Log = open(FILENAME_SPEEDPFC1_LOG, O_RDONLY | O_CREAT);
    int speedPFC2Log = open(FILENAME_SPEEDPFC2_LOG, O_RDONLY | O_CREAT);
    int speedPFC3Log = open(FILENAME_SPEEDPFC3_LOG, O_RDONLY | O_CREAT);
    FILE *status = open_file(FILENAME_STATUS_LOG, "w+");
    int fd = connectPipe(FILENAME_WES_PIPE, O_WRONLY);

    double velocita_pfc1;
    double velocita_pfc2;
    double velocita_pfc3;

    for(int i=0; i<50; i++) {
        sleep(1);

        velocita_pfc1 = readSpeed(speedPFC1Log);
        velocita_pfc2 = readSpeed(speedPFC2Log);
        velocita_pfc3 = readSpeed(speedPFC3Log);

        /*
         * TODO: controllare che velocita_pfc1, velocita_pfc2
         *       e velocita_pfc3 abbiano valori diversi da '\0'
         */

        if(velocita_pfc1 == velocita_pfc2) {
            if(velocita_pfc1 == velocita_pfc3) {
                printf("OK\n");
                fwrite("OK\n", sizeof(char), 3, status);
            } else {
                printf("ERRORE-PFC3\n");
                fwrite("ERRORE-PFC3\n", sizeof(char), 12, status);
                write(fd, "ERRORE-PFC3\0", sizeof(char) * 12);
            }
        } else {
            if(velocita_pfc1 == velocita_pfc3) {
                printf("ERRORE-PFC2\n");
                fwrite("ERRORE-PFC2\n", sizeof(char), 12, status);
                write(fd, "ERRORE-PFC2\0", sizeof(char) * 12);
            } else if(velocita_pfc2 == velocita_pfc3) {
                printf("ERRORE-PFC1\n");
                fwrite("ERRORE-PFC1\n", sizeof(char), 12, status);
                write(fd, "ERRORE-PFC1\0", sizeof(char) * 12);
            } else {
                printf("EMERGENZA\n");
                fwrite("EMERGENZA\n", sizeof(char), 10, status);
                write(fd, "EMERGENZA\0", sizeof(char) * 10);
            }
        }
    }

    close(speedPFC1Log);
    close(speedPFC2Log);
    close(speedPFC3Log);
    close(fd);
    fclose(status);

    return 0;
}

double readSpeed(int fd) {
    double speed;
    char *buffer = calloc(10, sizeof(char));

    readLine(fd, buffer, '\n');
    speed = strtod(buffer, NULL);
    free(buffer);

    return speed;
}
