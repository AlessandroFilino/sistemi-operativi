#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include "../include/utility.h"

int createChild(int (*execv_function)(const char*, char* const*), char *filename, char **argv) {
    int pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork Failed\n");
        return -1;
    } else if(pid == 0) {
        if(execv_function(filename, argv) == -1) {
            return -1;
        }
    }

    return pid;
}

int numberOfDigits(int value) {
    int digits = 0;

    do {
        value /= 10;
        digits++;
    } while (value != 0);

    return digits;
}

void removeLastChar(char *string) {
    unsigned long length = strlen(string);
    *(string + (length-1)) = 0;
}

double inverse(double number) {
    return (double) 1/number;
}

void tokenize(char *string, char *separator, int tokenNumber, ...) {
    char *argument;
    char *temp = string;

    va_list listOfArgs;
    va_start(listOfArgs, tokenNumber);

    for(int i=0; i<tokenNumber; i++) {
        argument = va_arg(listOfArgs, char *);

        if(argument != NULL) {
            strcpy(argument, strtok(temp, separator));
        }

        if(i == 0) {
            temp = NULL;
        }
    }

    va_end(listOfArgs);
}


