#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/errors.h"
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
                printf(CONCAT(WES_MESSAGE_SUCCESS, "\n"));
                fwrite(CONCAT(WES_MESSAGE_SUCCESS, "\n"), sizeof(char), strlen(WES_MESSAGE_SUCCESS)+1, status);
            } else {
                printf(CONCAT(WES_MESSAGE_PFC3_ERROR, "\n"));
                fwrite(CONCAT(WES_MESSAGE_PFC3_ERROR, "\n"), sizeof(char), strlen(WES_MESSAGE_PFC3_ERROR)+1, status);
                write(fd, WES_MESSAGE_PFC3_ERROR, sizeof(char) * strlen(WES_MESSAGE_PFC3_ERROR));
            }
        } else {
            if(velocita_pfc1 == velocita_pfc3) {
                printf(CONCAT(WES_MESSAGE_PFC2_ERROR, "\n"));
                fwrite(CONCAT(WES_MESSAGE_PFC2_ERROR, "\n"), sizeof(char), strlen(WES_MESSAGE_PFC2_ERROR)+1, status);
                write(fd, WES_MESSAGE_PFC2_ERROR, sizeof(char) * strlen(WES_MESSAGE_PFC2_ERROR));
            } else if(velocita_pfc2 == velocita_pfc3) {
                printf(CONCAT(WES_MESSAGE_PFC1_ERROR, "\n"));
                fwrite(CONCAT(WES_MESSAGE_PFC1_ERROR, "\n"), sizeof(char), strlen(WES_MESSAGE_PFC1_ERROR)+1, status);
                write(fd, WES_MESSAGE_PFC1_ERROR, sizeof(char) * strlen(WES_MESSAGE_PFC1_ERROR));
            } else {
                printf(CONCAT(WES_MESSAGE_EMERGENCY, "\n"));
                fwrite(CONCAT(WES_MESSAGE_EMERGENCY, "\n"), sizeof(char), strlen(WES_MESSAGE_EMERGENCY) + 1, status);
                write(fd, WES_MESSAGE_EMERGENCY, sizeof(char) * strlen(WES_MESSAGE_EMERGENCY));
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
