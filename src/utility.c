#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/utility.h"

FILE *open_file(const char* filename, const char* mode) {
    FILE *fp = fopen(filename, mode);

    if (fp == NULL) {
        //TODO come gestire gli errori?

        fprintf(stderr, "File non trovato\n");
        exit(EXIT_FAILURE);
    }

    return fp;
}

//TODO sistemare readline() usando realloc
int readLine(int fd, char *buffer, char delimiter) {
    int n;
    int count = 0;

    do { /* Read characters until ’\0’ or end-of-input */
        n = read (fd, buffer, 1); /* Read one character */
        count += n;
    } while (n > 0 && *buffer++ != delimiter);

    return count;
}

//TODO implementare scrittura con fprintf e dprintf

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

int connectPipe(char *pipename, int mode) {
    int fd;

    do {
        fd = open(pipename, mode);
        if (fd == -1) {
            printf("trying to connect...\n");
            usleep((1 * 1000) * 400); //400 millisecondi
        }
    } while (fd == -1);
    printf("connected!\n");

    return fd;
}

int connectSocket(int clientFd, const struct sockaddr* serverSockAddrPtr, socklen_t serverLen) {
    int result;

    do {
        result = connect(clientFd, serverSockAddrPtr, serverLen);
        if (result == -1) {
            printf("trying to connect...\n");
            usleep((1 * 1000) * 400); //400 millisecondi
        }
    } while(result == -1);
    printf("connected!\n");

    return result;
}

int digits_number(int number) {
    int digits = 0;

    do {
        number /= 10;
        digits++;
    } while (number != 0);

    return digits;
}

enum boolean tokenize(char *string, char *separator, int tokenNumber, char *buffer[]) {
    char *token;
    char *temp = string;

    for(int i=0; i<tokenNumber; i++) {
        token = strtok(temp, separator);
        if(token == NULL){
            return FALSE;
        }

        buffer[i] = token;

        if(i == 0) {
            temp = NULL;
        }
    }

    return TRUE;
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


