#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "../include/pfc.h"
#include "../include/utility.h"

void setSignalStatus(int signalReceived, enum boolean *PFC_sigUsr, enum boolean *PFC_sigRestart) {
    switch(signalReceived) {
        case SIGUSR1: {
            *PFC_sigUsr = TRUE;
            break;
        }

        case SIGUSR2:

        case SIGCONT: {
            printf("ciaociaociao - %d\n", signalReceived);
            fflush(stdout);
            
            *PFC_sigRestart = TRUE;
            break;
        }
    }
}

//TODO convertire questa funzione in una generica scrittura sul file e inserirla in utility.c
void addLastRead(FILE *lastRead, long current_position) {
    /* bufferPosition è un buffer che contiene la posizione letta da last_read sottoforma di stringa */
    char bufferPosition[MAX_G18_FILE_LENGTH_DIGITS] = {0};

    unsigned long numberOfCharsRead = fread(bufferPosition, sizeof(char), MAX_G18_FILE_LENGTH_DIGITS, lastRead);
    long lastReadPosition = strtol(bufferPosition, NULL, 10);

    //TODO gestire errore read == -1 (o in generale se read < 0)
    if(numberOfCharsRead == 0 || current_position > lastReadPosition) {
        fseek(lastRead, 0, SEEK_SET);

        fprintf(lastRead, "%ld", current_position);
        fflush(lastRead);
    }
}

void changePointerPosition(FILE *fp_g18, FILE *lastRead) {
    /* buffer_position è un buffer che contiene la posizione letta da lastRead sottoforma di stringa */
    char bufferPosition[MAX_G18_FILE_LENGTH_DIGITS] = {0};

    long position = 0;
    unsigned long numberOfCharsRead = fread(bufferPosition, sizeof(char), MAX_G18_FILE_LENGTH_DIGITS, lastRead);

    if(numberOfCharsRead > 0) {
        position = strtol(bufferPosition, NULL, 10);
        fseek(fp_g18, position, SEEK_SET);
    }
}

int setPreviousGeographicCoordinates(FILE *fp_g18, double *previousLatitude, double *previousLongitude) {
    ssize_t read = 0;
    size_t bufferLength = MAX_LINE_LENGTH + 1;
    char *buffer = malloc(sizeof(char) * bufferLength);

    read = readCorrectLine(buffer, bufferLength, fp_g18);
    getGeographicCoordinates(buffer, previousLatitude, previousLongitude);

    free(buffer);

    return read;
}

//TODO changeSpeed() la tengo qui o in generatoreFallimenti?
double changeSpeed(double speed) {
    return (double) (((int) round(speed)) << 2);
}

ssize_t readCorrectLine(char *buffer, size_t bufferLength, FILE *fp) {
    /*
     * char *strstr(const char *haystack, const char *needle)
     *
     * finds the first occurrence of the substring needle in the string haystack.
     * This function returns a pointer to the first occurrence in haystack of any
     * of the entire sequence of characters specified in needle, or a null pointer
     * if the sequence is not present in haystack
     */
    ssize_t read;
    //TODO leggere carattere per carattere e usare realloc
    do {
        read = getline(&buffer, &bufferLength, fp);
    } while(read != -1 && strstr(buffer, NMEA_FORMAT) == NULL);

    return read;
}

void getGeographicCoordinates(char* line, double* latitude, double* longitude) {
    //TODO usare la funzione tokenize di utility.c
    char* tok;
    char latitudeLocation[2] = {0};
    char longitudeLocation[2] = {0};

    tok = strtok(line, SEPARATOR_TOKENIZER);

    //TODO gestire errore
    if(tok == NULL) {
        fprintf(stderr, "Errore\n");
    }

    //char temp_latitude[50] = {0}
    //char temp_longitude[50] = {0}
    //tokenize(line, SEPARATOR_TOKENIZER, 3, temp_latitude, direzioneLatitudine, temp_longitude, direzioneLongitudine);

    /*
     * I valori di Latitudine e longitudine vengono divisi per 100
     * perchè la funzione getDistance() accetta valori in quel formato
     *
     * I gradi presi rispetto a Sud (cioè 'S') e a Ovest (cioè 'W')
     * devono essere negativi per calcolare correttamente la distanza
     */
    *latitude = strtod(strtok(NULL, SEPARATOR_TOKENIZER), NULL)/100;
    strcpy(latitudeLocation, strtok(NULL, SEPARATOR_TOKENIZER));
    if(latitudeLocation[0] == 'S' || latitudeLocation[0] == 'W') {
        *latitude *= -1;
    }

    *longitude = strtod(strtok(NULL, SEPARATOR_TOKENIZER), NULL)/100;
    strcpy(longitudeLocation, strtok(NULL, SEPARATOR_TOKENIZER));
    if(longitudeLocation[0] == 'S' || longitudeLocation[0] == 'W') {
        *longitude *= -1;
    }
}

double degreeToRadian(double degree) {
    return degree * ((double) M_PI / 180);
}

double getDistance(double latitude, double longitude, double previousLatitude, double previousLongitude){
    //TODO rinominare "dLat" in "differenceLatitude" e "dLon" in "differenceLongitude"?

    double dLat = degreeToRadian(latitude - previousLatitude);
    double dLon = degreeToRadian(longitude - previousLongitude);
    latitude = degreeToRadian(latitude);
    previousLatitude = degreeToRadian(previousLatitude);

    double a = sin(dLat/2) * sin(dLat/2) +
               sin(dLon/2) * sin(dLon/2) * cos(previousLatitude) * cos(latitude);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return RAGGIO_TERRA_METRI * c;
}

double getVelocity(double space, int time){
    /* Spazio in metri, time in secondi m/s */
    return space / time;
}

int exe(int fd_pfcToTransducers, FILE *fp_g18, FILE *last_read, double *previousLatitude, double *previousLongitude, enum boolean *sigUsr, enum boolean *sigRestart) {
    double latitude;
    double longitude;
    double distance;
    double velocity;

    size_t lineLength = MAX_LINE_LENGTH + 1;
    char *line = malloc(sizeof(char) * lineLength);
    ssize_t read = readCorrectLine(line, lineLength, fp_g18);

    if (read > 0) {
        getGeographicCoordinates(line, &latitude, &longitude);
        distance = getDistance(latitude, longitude, *previousLatitude, *previousLongitude);
        velocity = getVelocity(distance, TEMPO);

        if (*sigUsr == TRUE) {
            velocity = changeSpeed(velocity);
            *sigUsr = FALSE;
        }

        int int_section = (int) velocity;
        int digits = numberOfDigits(int_section);

        /*
         * size_t is an unsigned integer type of at least 16 bit.
         * In questo caso, è necessario prevedere un buffer che contenga:
         *      • tutte le cifre della parte intera del numero (digits)
         *      • il punto che divide parte intera e parte decimale (1)
         *      • le due cifre decimali (2)
         */
        size_t messageLength = digits + 1 + 2;

        /*
         * se il numero è negativo va predisposto
         * lo spazio per contenere il carattere
         * del segno
         */
        //TODO la velcita può essere negativa solo se read == -1, giusto?
        if (velocity < 0) {
            messageLength++;
        }

        char *message = malloc(sizeof(char) * (messageLength + 1 + 1)); //+2 per il carattere \n e poi di \0 (fine stringa)
        int result = snprintf(message, sizeof(char) * (messageLength + 1 + 1), "%.2f\n", velocity);

        if (result < 0) {
            return -1;
        }

        if (*sigRestart) {
            changePointerPosition(fp_g18, last_read);
            setPreviousGeographicCoordinates(fp_g18, previousLatitude, previousLongitude);
            *sigRestart = FALSE;
        } else {
            /*
             * TODO: sizeof(char) potrebbe essere eliminato in quanto
             *       la write scrive su un file leggendo da una stringa
             */

            write(fd_pfcToTransducers, message, sizeof(char) * (messageLength + 1));
            //printf("%s", message);

            *previousLatitude = latitude;
            *previousLongitude = longitude;
            addLastRead(last_read, ftell(fp_g18));
        }

        free(message);
    }

    free(line);

    return read;
}

