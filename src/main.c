#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/config.h"
#include "../include/messages.h"

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
    createChild(&execv, "transducers", transducers_argv);

    char *pfcDisconnectedSwitch_argv[] = {"pfcDisconnectedSwitch", filename_g18, NULL};
    createChild(&execv, "pfcDisconnectedSwitch", pfcDisconnectedSwitch_argv);
	
	/*
	 * La sleep permette a PFC1, PFC2 e PFC3 di scrivere un valore
	 * nei rispettivi file di log consentendo una lettura valida
	 * al processo WES
	 */
     
    //sleep(1);
	usleep(1000 * 800);

    char *wes_argv[] = {"wes", NULL};
    createChild(&execv, "wes", wes_argv);

    for(int i=0; i<3; i++) {
        pid = wait(&status);

        /*
         * Se WIFEXITED(status) != 0, allora un processo figlio
         * è terminato con un errore
         */
        if (!WIFEXITED(status)) {
            printf("main: Child process %d exited with %d status\n", pid, WEXITSTATUS(status));
        }
    }

    return 0;
}


