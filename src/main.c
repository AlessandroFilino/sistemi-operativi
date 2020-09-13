#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/config.h"
#include "../include/messages.h"

//TODO spostare la creazione delle pipe nel main

int main(int argc, const char* argv[]) {
    int status;
    int pid = 0;

    unsigned int pathnameLength = strlen(argv[1]) + 1;
    char *filename_g18 = malloc(sizeof(char) * pathnameLength);
    strcpy(filename_g18, argv[1]);

    if(system(remove_files_from(PATHNAME_TEMP)) || system(remove_files_from(PATHNAME_LOG)) != 0) {
        fprintf(stderr, "%s\n", MAIN_PATH_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    }

    if(access(filename_g18, F_OK) == -1) {
        fprintf(stderr, "%s\n", MAIN_INPUT_FNF_MESSAGE);
        exit(EXIT_FAILURE);
    }

    createEmptyFile(FILENAME_LAST_READ, "w");

    char *transducers_argv[] = {"transducers", NULL};
    int transducers_pid = createChild(&execv, "transducers", transducers_argv);

    char *pfcDisconnectedSwitch_argv[] = {"pfcDisconnectedSwitch", filename_g18, NULL};
    int pfcDisconnectedSwitch_pid = createChild(&execv, "pfcDisconnectedSwitch", pfcDisconnectedSwitch_argv);
	
	//La sleep permette a PFC1, PFC2 e PFC3 di scrivere un valore 
	//nei rispettivi file di log consentendo una lettura valida
	//al processo WES 
	
     
    //sleep(1);
	usleep(1000 * 800);

    char *wes_argv[] = {"wes", NULL};
    int wes_pid = createChild(&execv, "wes", wes_argv);

    while(pid >= 0) {
        pid = wait(&status);

        //Child process exited normally, through `return` or `exit`
        if (WIFEXITED(status)) {
            char name[50] = {0};

            if(pid == pfcDisconnectedSwitch_pid) {
                strcpy(name, "pfcDisconnectedSwitch");
            } else if(pid == transducers_pid) {
                strcpy(name, "transducers");
            } else if(pid == wes_pid) {
                strcpy(name, "wes");
            } else {
                snprintf(name, sizeof(char) * 10, "%d", pid);
            }
            
            printf("main: Child process '%s' exited with %d status\n", name, WEXITSTATUS(status));
        }
    }

    return 0;
}


