#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include <unistd.h>

int main(int argc, const char* argv[]) {
    /*
     * argv[1] --> g18.txt
     */
    char *filename = "../sistemioperativi/doc/G18.txt";

    if(fork() == 0) {
        if(execl("pfcDisconnectedSwitch","pfcDisconnectedSwitch", filename, NULL) == -1) {
            exit(EXIT_FAILURE);
        }
    }

    int status;
    wait(&status);

    return 0;
}
