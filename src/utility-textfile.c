#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/utility-textfile.h"

void createEmptyFile(char *filename, char *mode) {
    fclose(openFile(filename, mode));
}

FILE *openFile(const char* filename, const char* mode) {
    FILE *fp = fopen(filename, mode);

    if (fp == NULL) {
        fprintf(stderr, "File non trovato: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    return fp;
}

int readLine(int fd, char *buffer, char delimiter) {
    int n = 0;
    int count = 0;

    do {
        n = read(fd, buffer, 1);
        count += n;
    } while (n > 0 && *buffer++ != delimiter);

    return count;
}

