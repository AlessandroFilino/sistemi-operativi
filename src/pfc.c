#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "../include/config.h"
#include "../include/pfc.h"
#include "../include/utility-math.h"

void setSignalStatus(int signalReceived, enum boolean *PFC_sigUsr, enum boolean *PFC_sigRestart) {
    switch(signalReceived) {
        case SIGUSR1: {
            *PFC_sigUsr = TRUE;
            break;
        }

        case SIGCONT: {
            *PFC_sigRestart = TRUE;
            break;
        }
    }
}

void addLastRead(long current_position, FILE *lastRead) {
	unsigned long numberOfCharsRead = 0;
	long lastReadPosition = 0;

    /* bufferPosition è un buffer che contiene la posizione letta da last_read sottoforma di stringa */
    char bufferPosition[MAX_G18_FILE_LENGTH_DIGITS + 1] = {0};

	fseek(lastRead, 0, SEEK_SET);
   	numberOfCharsRead = fread(bufferPosition, sizeof(char), MAX_G18_FILE_LENGTH_DIGITS, lastRead);
   	lastReadPosition = strtol(bufferPosition, NULL, 10);

    if(numberOfCharsRead == 0 || current_position > lastReadPosition) {
        fseek(lastRead, 0, SEEK_SET);

        fprintf(lastRead, "%ld", current_position);
        fflush(lastRead);
    }
}

void changePointerPosition(FILE *fp_g18, FILE *lastRead) {
    long position = 0;
    unsigned long numberOfCharsRead = 0;

    /* buffer_position è un buffer che contiene la posizione letta da lastRead sottoforma di stringa */
    char bufferPosition[MAX_G18_FILE_LENGTH_DIGITS + 1] = {0};

    fseek(lastRead, 0, SEEK_SET);
    numberOfCharsRead = fread(bufferPosition, sizeof(char), MAX_G18_FILE_LENGTH_DIGITS, lastRead);

    if(numberOfCharsRead > 0) {
        position = strtol(bufferPosition, NULL, 10);
        fseek(fp_g18, position, SEEK_SET);
    }
}

int setPreviousGeographicCoordinates(FILE *fp_g18, double *previousLatitude, double *previousLongitude) {
    ssize_t read = 0;
    size_t bufferLength = MAX_LINE_LENGTH + 1;
    char *buffer = malloc(sizeof(char) * bufferLength);
    int position = 0;
    int offset = 0;

    if(ftell(fp_g18) < MAX_NMEA_FORMAT_LINE_LENGTH) {
	    position = SEEK_SET;
	    offset = 0;
    } else {
	    position = SEEK_CUR;
	    offset = -50;
    }

    fseek(fp_g18, offset, position);
	read = readCorrectLine(buffer, bufferLength, fp_g18);

	if(read > 0) {
        getGeographicCoordinates(buffer, previousLatitude, previousLongitude);
	}
    
    free(buffer);

    return read;
}

double changeSpeed(double speed) {
    return (double) (((int) round(speed)) << 2);
}

ssize_t readCorrectLine(char *buffer, size_t bufferLength, FILE *fp) {
    /*
     * La funzione strstr(const char *haystack, const char *needle)
     * permette di trovare la prima occorrenza della sottostringa needle
     * all'interno della stringa haystack. Se nella riga letta è presente
     * la stringa "$GPGLL", allora va usata per calcolare la velocità
     */

    ssize_t read;

    do {
        read = getline(&buffer, &bufferLength, fp);
    } while(read != -1 && strstr(buffer, NMEA_FORMAT) == NULL);

    return read;
}

void getGeographicCoordinates(char* line, double* latitude, double* longitude) {
    char latitudeLocation[5] = {0};
    char longitudeLocation[5] = {0};
    char temp_latitude[GEOGRAPHIC_COORDINATES_LENGTH + 1] = {0};
    char temp_longitude[GEOGRAPHIC_COORDINATES_LENGTH + 1] = {0};
    char temp_nmea[string_length(NMEA_FORMAT) + 1] = {0};

    tokenize(line, NMEA_FORMAT_SEPARATOR, 5, &temp_nmea, &temp_latitude, &latitudeLocation, &temp_longitude, &longitudeLocation);

    /*
     * I valori di Latitudine e longitudine vengono divisi per 100
     * perchè la funzione getDistance() accetta valori in quel formato
     *
     * I gradi presi rispetto a Sud (cioè 'S') e a Ovest (cioè 'W')
     * devono essere negativi per calcolare correttamente la distanza
     */

    *latitude = strtod(temp_latitude, NULL)/100;
    if(latitudeLocation[0] == 'S' || latitudeLocation[0] == 'W') {
        *latitude *= -1;
    }

    *longitude = strtod(temp_longitude, NULL)/100;
    if(longitudeLocation[0] == 'S' || longitudeLocation[0] == 'W') {
        *longitude *= -1;
    }
}

int exe(int fd_pfcToTransducers, FILE *fp_g18, FILE *lastRead, double *previousLatitude, double *previousLongitude, enum boolean *sigUsr, enum boolean *sigRestart) {
    double latitude = 0;
    double longitude = 0;
    double distance = 0;
    double speed = 0;

    char line[MAX_LINE_LENGTH + 1] = {0};
    ssize_t read = readCorrectLine(line, MAX_LINE_LENGTH, fp_g18);

    if (read > 0) {
        getGeographicCoordinates(line, &latitude, &longitude);
        distance = getDistance(latitude, longitude, *previousLatitude, *previousLongitude);
        speed = getVelocity(distance, TEMPO);

        if (*sigUsr == TRUE) {
            speed = changeSpeed(speed);
            *sigUsr = FALSE;
        }

        int int_section = (int) speed;
        int digits = numberOfDigits(int_section);

        /*
         * Per la velocità è necessario prevedere un buffer che contenga:
         *      • tutte le cifre della parte intera del numero (digits)
         *      • il punto che divide parte intera e parte decimale (1)
         *      • le due cifre decimali (2)
         */
        size_t messageLength = digits + 1 + 2;

        /*
         * Il buffer message deve contenere l'intero messaggio da inviare al transducers
         * (la velocità sottodorma di stringa) ma deve comprendere anche i caratteri
         * '\n' (per distinguere i singoli valori delle velocità nel momento della
         * lettura attraverso il canale di comunicazione, qualsiasi esso sia) e '\0'
         * (ovvero il carattere di fine stringa)
         */
        char *message = malloc(sizeof(char) * (messageLength + 1 + 1));
        snprintf(message, sizeof(char) * (messageLength + 1 + 1), "%.2f\n", speed);

        if (*sigRestart) {
            changePointerPosition(fp_g18, lastRead);
            setPreviousGeographicCoordinates(fp_g18, previousLatitude, previousLongitude);

            *sigRestart = FALSE;
        } else {
            write(fd_pfcToTransducers, message, sizeof(char) * (messageLength + 1));

            *previousLatitude = latitude;
            *previousLongitude = longitude;
            addLastRead(ftell(fp_g18), lastRead);
        }

        free(message);
    }

    return read;
}

