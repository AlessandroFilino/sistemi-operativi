#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "../include/pfc.h"
#include "../include/utility.h"

void changeSigusr(enum boolean *sigusr) {
    *sigusr = !(*sigusr);
}

//TODO convertire questa funzione in una generica scrittura sul file e inserirla in utility.c
void addLastRead(int fd_last_read, long current_position) {
    /* buffer_position è un buffer che contiene la posizione letta da last_read sottoforma di stringa */
    char buffer_position[MAX_G18_FILE_LENGTH_DIGITS] = {0};

    readLine(fd_last_read, buffer_position, EOF);
    long last_read_position = strtol(buffer_position, NULL, 10);

    if(current_position > last_read_position) {
        dprintf(fd_last_read, "%ld", current_position);
    }
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

void acquisisciCoordinate(char* line, double* latitudine, double* longitudine){
    char* tok;
    tok = strtok(line, ",");

    if(tok == NULL){
        fprintf(stderr, "Errore\n");
        exit (EXIT_FAILURE);
    }

    *latitudine = atof(strtok(NULL, ","));
    strtok(NULL, ","); //direzioneLatitudine (da scartare)
    *longitudine = atof(strtok(NULL, ","));
    strtok(NULL, ","); //direzioneLongitudine (da scartare)
}

double gradiToRadianti(double gradi) {
    return gradi * M_PI / 180;
}

double calcoloDistanza(double latitudine, double longitudine, double latitudine_prec, double longitudine_prec){
    double dLat = gradiToRadianti(latitudine - latitudine_prec);
    double dLon = gradiToRadianti(longitudine - longitudine_prec);
    latitudine = gradiToRadianti(latitudine);
    latitudine_prec = gradiToRadianti(latitudine_prec);

    double a = sin(dLat/2) * sin(dLat/2) +
               sin(dLon/2) * sin(dLon/2) * cos(latitudine_prec) * cos(latitudine);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return RAGGIO_TERRA_METRI * c;
}

double calcoloVelocita(double spazio, int tempo){
    /* Spazio in metri, tempo in secondi m/s */
    return spazio/tempo;
}

int exe(int fd_pfc_to_transducers, FILE *fp_g18, int last_read, double *latitudine_prec, double *longitudine_prec, enum boolean *sigusr) {
    double latitudine;
    double longitudine;
    double distance;
    double speed;

    size_t lineLength = MAX_LINE_LENGTH + 1;
    char *line = malloc(sizeof(char) * lineLength);
    ssize_t read = readCorrectLine(line, lineLength, fp_g18);

    if(read != -1) {
        acquisisciCoordinate(line, &latitudine, &longitudine);
        distance = calcoloDistanza(latitudine, longitudine, *latitudine_prec, *longitudine_prec);
        speed = calcoloVelocita(distance, TEMPO);

        if(*sigusr == TRUE) {
            speed = changeSpeed(speed);
            changeSigusr(sigusr);
        }
    } else {
        speed = -1;
    }

    int int_section = (int) speed;
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
    if(speed < 0) {
        messageLength++;
    }

    char *message = malloc(sizeof(char) * (messageLength + 1)); //+1 per il carattere di fine stringa
    int result = snprintf(message, sizeof(char) * (messageLength + 1), "%.2f", speed);

    if(result < 0) {
        return -1;
    }

    /*
     * TODO: sizeof(char) potrebbe essere eliminato in quanto
     *       la write scrive su un file leggendo da una stringa
     */

    write(fd_pfc_to_transducers, message, sizeof(char) * (messageLength + 1));
    printf("%s\n", message);

    *latitudine_prec = latitudine;
    *longitudine_prec = longitudine;
    addLastRead(last_read, ftell(fp_g18));

    free(line);
    free(message);

    return read;
}

