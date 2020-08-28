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

        case SIGSTOP: {
            *PFC_sigRestart = TRUE;
            break;
        }
    }
}

//TODO convertire questa funzione in una generica scrittura sul file e inserirla in utility.c
void addLastRead(int fd_last_read, long current_position) {
    /* buffer_position è un buffer che contiene la posizione letta da last_read sottoforma di stringa */
    char buffer_position[MAX_G18_FILE_LENGTH_DIGITS] = {0};

    readLine(fd_last_read, buffer_position, EOF);
    //TODO al posto di strtol usare
    long last_read_position = strtol(buffer_position, NULL, 10);

    if(current_position > last_read_position) {
        dprintf(fd_last_read, "%ld", current_position);
    }
}

void changePointerPosition(FILE *fp_g18, int last_read) {
    /* buffer_position è un buffer che contiene la posizione letta da last_read sottoforma di stringa */
    char buffer_position[MAX_G18_FILE_LENGTH_DIGITS] = {0};
    long position = 0;
    int bytes = read(last_read, buffer_position, MAX_G18_FILE_LENGTH_DIGITS);

    if(bytes != 0) {
        position = strtol(buffer_position, NULL, 10);
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

void getGeographicCoordinates(char* line, double* latitude, double* longitude){
    char* tok;
    tok = strtok(line, ",");

    if(tok == NULL){
        fprintf(stderr, "Errore\n");
        exit (EXIT_FAILURE);
    }

    *latitude = atof(strtok(NULL, ","));
    strtok(NULL, ","); //direzioneLatitudine (da scartare)
    *longitude = atof(strtok(NULL, ","));
    strtok(NULL, ","); //direzioneLongitudine (da scartare)
}

double degreeToRadian(double degree) {
    return degree * M_PI / 180;
}

double getDistance(double latitude, double longitude, double previousLatitude, double previousLongitude){
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

int exe(int fd_pfcToTransducers, FILE *fp_g18, int last_read, double *previousLatitude, double *previousLongitude, enum boolean *sigUsr, enum boolean *sigRestart) {
    double latitude;
    double longitude;
    double distance;
    double velocity;

    size_t lineLength = MAX_LINE_LENGTH + 1;
    char *line = malloc(sizeof(char) * lineLength);
    ssize_t read = readCorrectLine(line, lineLength, fp_g18);

    if (read != -1) {
        getGeographicCoordinates(line, &latitude, &longitude);
        distance = getDistance(latitude, longitude, *previousLatitude, *previousLongitude);
        velocity = getVelocity(distance, TEMPO);

        if (*sigUsr == TRUE) {
            velocity = changeSpeed(velocity);
            *sigUsr = FALSE;
        }
    } else {
        velocity = -1;
    }

    int int_section = (int) velocity;
    int digits = digits_number(int_section);

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
    if (velocity < 0) {
        messageLength++;
    }

    char *message = malloc(sizeof(char) * (messageLength + 1)); //+1 per il carattere di fine stringa
    int result = snprintf(message, sizeof(char) * (messageLength + 1), "%.2f", velocity);

    if (result < 0) {
        return -1;
    }

    /*
     * TODO: sizeof(char) potrebbe essere eliminato in quanto
     *       la write scrive su un file leggendo da una stringa
     */

    write(fd_pfcToTransducers, message, sizeof(char) * (messageLength + 1));
    printf("%s\n", message);

    if(*sigRestart) {
        changePointerPosition(fp_g18, last_read);
        setPreviousGeographicCoordinates(fp_g18, previousLatitude, previousLongitude);
        *sigRestart = FALSE;
    } else {
        *previousLatitude = latitude;
        *previousLongitude = longitude;
        addLastRead(last_read, ftell(fp_g18));
    }

    free(line);
    free(message);

    return read;
}

