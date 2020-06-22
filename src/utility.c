#include <stdlib.h>
#include <unistd.h>
#include "utility.h"

FILE *open_file(const char* filename, const char* mode) {
    FILE *fp = fopen(filename, mode);

    if (fp == NULL){
        //TODO come gestire gli errori?

        fprintf(stderr, "File non trovato\n");
        exit(EXIT_FAILURE);
    }

    return fp;
}

//TODO sistemare readline() usando realloc
int readLine(int fd, char *buffer) {
    int n;
    int count = 0;

    do { /* Read characters until ’\0’ or end-of-input */
        n = read (fd, buffer, 1); /* Read one character */
        count++;
    } while (n > 0 && *buffer++ != '\0');

    return count;
}

int createChild(int (*execv_function)(const char*, char* const*), char *filename, char **argv) {
    int pid = fork();

    if (pid < 0) { /* error occurred */
        //TODO usare perror

        fprintf(stderr, "Fork Failed\n");
        exit(EXIT_FAILURE);
    } else if(pid == 0) {
        if(execv_function(filename, argv) == -1) {
            exit(EXIT_FAILURE);
        }
    }

    return pid;
}

int digits_number(int number) {
    int digits = 0;

    do {
        number /= 10;
        digits++;
    } while (number != 0);

    return digits;
}

void intToString(char *buffer, int buffer_size, int number) {
    if(snprintf(buffer, sizeof(char) * buffer_size, "%d", number) < 0){
        //TODO stampare errore
    }
}

int doubleToString(char *buffer, int buffer_size, double number) {
    if(snprintf(buffer, sizeof(char) * buffer_size, "%.2f", number) < 0){
        return FALSE;
    }

    return TRUE;
}


