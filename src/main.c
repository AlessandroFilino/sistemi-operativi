#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/utility.h"
#include "../include/main.h"

int main(int argc, const char* argv[]) {
    /*
     * argv[1] --> g18.txt
     */

    /*
     * TODO mettere path (in tutti i file.c):
     *      char pathname[50] = {0};
     *      strcpy(pathname, PATHNAME_LOG/PATHNAME_TEMP);
     *      strcat(pathname, "filename");
     */

    /*char command1[100] = {0}; //86 caratteri utilizzati
    sprintf(command1, COMMAND_REMOVE_FILES, PATHNAME_TEMP, PATHNAME_TEMP);

    char command2[100] = {0}; //84 caratteri utilizzati
    sprintf(command2, COMMAND_REMOVE_FILES, PATHNAME_LOG, PATHNAME_LOG);

    if(system(command1) || system(command2) != 0) {
        fprintf(stderr, "%s", "main: path error\n");
        exit(EXIT_FAILURE);
    }

    int status;
    char *filename_G18 = "../sistemioperativi/doc/G18.txt";

    if(access(filename_G18, F_OK) == -1) {
        char error[50];
        sprintf(error, "main: File %s not found\n", filename_G18);
        fprintf(stderr, "%s", error);
        exit(EXIT_FAILURE);
    }*/

    char line[10] = {0};
    int last_read = open("../sistemioperativi/temp/test.txt", O_CREAT | O_RDWR);
    int position = lseek(last_read, 0, SEEK_CUR);
    printf("%d", position);
    readLine(last_read, line, '\n');
    printf("\n%s", line);
    memset(line, 0, 10);

    readLine(last_read, line, '\n');
    printf("%s", line);
    memset(line, 0, 10);

    readLine(last_read, line, '\n');
    printf("%s", line);
    memset(line, 0, 10);
    //lseek(last_read, 0, SEEK_SET);


    /*char *pfcDisconnectedSwitch_argv[] = {"pfcDisconnectedSwitch", filename_G18, NULL};
    createChild(&execv, "pfcDisconnectedSwitch", pfcDisconnectedSwitch_argv);

    char *transducer_argv[] = {"transducer", NULL};
    createChild(&execv, "transducer", transducer_argv);

    char *wes_argv[] = {"wes", NULL};
    createChild(&execv, "wes", wes_argv);

    for(int i=0; i<2; i++) {
        wait(&status);
    }*/

    return 0;
}
