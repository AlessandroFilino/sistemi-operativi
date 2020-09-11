#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/utility.h"
#include "../include/main.h"
#include "../include/path.h"
#include "../include/messages.h"

//TODO: spostare getDistance() e getVelocity() in utility? 

//TODO spostare la creazione delle pipe nel main

/*
    TODO: possiamo creare un handler per il segnale di SIGSTOP
          che setta a TRUE una variabile e quando un pfc controlla
          tale variabile e risulta TRUE, si manda un segnale di SIGSTOP
          a se stesso, così possiamo bloccare il processo alla fine 
          dell'esecuzione della funzione exe() e così il processo, appena
          viene ripristinato, inizierà eseguendo la funzione exe()
          dall'inizio (usare la funzione raise())
*/

/*
    TODO: se nell'ultima riga da leggere uno dei pfc
          ha un errore, il pfcDisconnectedSwitch entra
          nell'ultimo else e crea un nuovo processo perchè
          avendo letto l'ultima riga, il pfc in questione
          è già terminato. Come risolvere?
*/

/*
    TODO: la funzione waitpid() in pfcDisconnectedSwitch restituisce
          un numero > 0 se c'è un figlio zombie o un figlio bloccato 
          (se usiamo le opzioni WNOHANG e WUNTRACED). La wait() del
          pfcdisconnectedSwitch restituisce -1 se non ci sono figli
          zombie e ciò accade quando generatoreFallimenti genera un 
          errore su uno dei processi pfc già terminato ed il processo
          zombie viene catturato da waitpid() (succede quando 
          l'errore viene generato a ridosso della fine di G18.txt)
          Se il processo considerato non cambia stato
          (ovvero se è in esecuzione), waitpid() restituisce
          zero. Altrimenti, se il processo è bloccato o zombie,
          restituisce il suo pid.
*/

int main(int argc, const char* argv[]) {
    int status;
    int pid = 0;
    char filename_G18[25] = {0};
    strcpy(filename_G18, argv[1]);

    if(system(remove_files_from(PATHNAME_TEMP)) || system(remove_files_from(PATHNAME_LOG)) != 0) {
        fprintf(stderr, "%s\n", MAIN_PATH_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    }

    if(access(filename_G18, F_OK) == -1) {
        fprintf(stderr, "%s\n", MAIN_INPUT_FNF_MESSAGE);
        exit(EXIT_FAILURE);
    }

    createEmptyFile(FILENAME_LAST_READ, "w");

    char *transducers_argv[] = {"transducers", NULL};
    int transducers_pid = createChild(&execv, "transducers", transducers_argv);

    char *pfcDisconnectedSwitch_argv[] = {"pfcDisconnectedSwitch", filename_G18, NULL};
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


