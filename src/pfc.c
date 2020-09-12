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

//TODO convertire questa funzione in una generica scrittura sul file e inserirla in utility.c
void addLastRead(long current_position, FILE *lastRead) {
	unsigned long numberOfCharsRead = 0;
	long lastReadPosition = 0;

    /* bufferPosition è un buffer che contiene la posizione letta da last_read sottoforma di stringa */
    char bufferPosition[MAX_G18_FILE_LENGTH_DIGITS] = {0};

	fseek(lastRead, 0, SEEK_SET);
   	numberOfCharsRead = fread(bufferPosition, sizeof(char), MAX_G18_FILE_LENGTH_DIGITS, lastRead);
    	lastReadPosition = strtol(bufferPosition, NULL, 10);

    //TODO gestire errore read == -1 (o in generale se read < 0)
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
    char bufferPosition[MAX_G18_FILE_LENGTH_DIGITS] = {0};

    if(fseek(lastRead, 0, SEEK_SET) < 0) {
	fprintf(stderr, "changePointerPosition: errore nella prima fseek\n");
    } else {
	    numberOfCharsRead = fread(bufferPosition, sizeof(char), MAX_G18_FILE_LENGTH_DIGITS, lastRead);

	    if(numberOfCharsRead > 0) {
		  position = strtol(bufferPosition, NULL, 10);
		  if(fseek(fp_g18, position, SEEK_SET) < 0) {
			fprintf(stderr, "changePointerPosition: errore nella seconda fseek\n");
		  }
	    } else {
		fprintf(stderr, "changePointerPosition: numberOfCharsRead <= 0\n");
	    }
	}
}

int setPreviousGeographicCoordinates(FILE *fp_g18, double *previousLatitude, double *previousLongitude) {
    ssize_t read = 0;
    size_t bufferLength = MAX_LINE_LENGTH + 1;
    char *buffer = malloc(sizeof(char) * bufferLength);
    int position = 0;
    int offset = 0;


    if(ftell(fp_g18) < 50) {
	position = SEEK_SET;
	offset = 0;
    } else {
	position = SEEK_CUR;
	offset = -50;
    }

	if(fseek(fp_g18, offset, position) < 0) {
		fprintf(stderr, "setPreviousGeographicCoordinates: errore nella prima fseek\n");
		return -1;
	}

	read = readCorrectLine(buffer, bufferLength, fp_g18);
	//printf("///// %s ", buffer);

	if(read < 0) {
		fprintf(stderr, "setPreviousGeographicCoordinates: errore nella seconda fseek\n");
	} else {
	    	getGeographicCoordinates(buffer, previousLatitude, previousLongitude);
	}
	    
    
    free(buffer);

    return read;
}

enum boolean checkCorrectPosition(FILE *fp_g18, FILE *lastRead) {
    long position = 0;
    unsigned long numberOfCharsRead = 0;
    enum boolean result = FALSE;

    /* buffer_position è un buffer che contiene la posizione letta da lastRead sottoforma di stringa */
    char bufferPosition[MAX_G18_FILE_LENGTH_DIGITS] = {0};
	
	if(fseek(lastRead, 0, SEEK_SET) < 0) {
		fprintf(stderr, "changePointerPosition: errore nella prima fseek\n");
      } else {
	    numberOfCharsRead = fread(bufferPosition, sizeof(char), MAX_G18_FILE_LENGTH_DIGITS, lastRead);

	    if(numberOfCharsRead > 0) {
		  position = strtol(bufferPosition, NULL, 10);

		  if(position == ftell(fp_g18)) {
			result = TRUE;
		  }
	    }
	}

	return result;
}

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

    tok = strtok(line, NMEA_FORMAT_SEPARATOR);

    //TODO gestire errore
    if(tok == NULL) {
        fprintf(stderr, "Errore\n");
    }

    //char temp_latitude[50] = {0}
    //char temp_longitude[50] = {0}
    //tokenize(line, NMEA_FORMAT_SEPARATOR, 3, temp_latitude, direzioneLatitudine, temp_longitude, direzioneLongitudine);

    /*
     * I valori di Latitudine e longitudine vengono divisi per 100
     * perchè la funzione getDistance() accetta valori in quel formato
     *
     * I gradi presi rispetto a Sud (cioè 'S') e a Ovest (cioè 'W')
     * devono essere negativi per calcolare correttamente la distanza
     */
    *latitude = strtod(strtok(NULL, NMEA_FORMAT_SEPARATOR), NULL)/100;
    strcpy(latitudeLocation, strtok(NULL, NMEA_FORMAT_SEPARATOR));
    if(latitudeLocation[0] == 'S' || latitudeLocation[0] == 'W') {
        *latitude *= -1;
    }

    *longitude = strtod(strtok(NULL, NMEA_FORMAT_SEPARATOR), NULL)/100;
    strcpy(longitudeLocation, strtok(NULL, NMEA_FORMAT_SEPARATOR));
    if(longitudeLocation[0] == 'S' || longitudeLocation[0] == 'W') {
        *longitude *= -1;
    }
}

int exe(int fd_pfcToTransducers, FILE *fp_g18, FILE *lastRead, double *previousLatitude, double *previousLongitude, enum boolean *sigUsr, enum boolean *sigRestart) {
    double latitude;
    double longitude;
    double distance;
    double velocity;

	
	/*if (*sigRestart) {
            changePointerPosition(fp_g18, lastRead);
            setPreviousGeographicCoordinates(fp_g18, previousLatitude, previousLongitude);
            *sigRestart = FALSE;
	}*/

    size_t lineLength = MAX_LINE_LENGTH + 1;
    char *line = malloc(sizeof(char) * lineLength);
    ssize_t read = readCorrectLine(line, lineLength, fp_g18);

	//printf("%s\n", line);
	//fflush(stdout);

    if (read > 0) {
	//printf("%s - ", line);
        getGeographicCoordinates(line, &latitude, &longitude);
        distance = getDistance(latitude, longitude, *previousLatitude, *previousLongitude);
        velocity = getVelocity(distance, TEMPO);

        if (*sigUsr == TRUE) {
            velocity = changeSpeed(velocity);
            *sigUsr = FALSE;
        }

	//printf("%.2f\n", velocity);
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
		//usleep(1000 * 1000);

		if(!checkCorrectPosition(fp_g18, lastRead)) {
            	changePointerPosition(fp_g18, lastRead);
            	setPreviousGeographicCoordinates(fp_g18, previousLatitude, previousLongitude);
		}

            *sigRestart = FALSE;
	} else {
            /*
             * TODO: sizeof(char) potrebbe essere eliminato in quanto
             *       la write scrive su un file leggendo da una stringa
             */

            int t = write(fd_pfcToTransducers, message, sizeof(char) * (messageLength + 1));

            *previousLatitude = latitude;
            *previousLongitude = longitude;
            addLastRead(ftell(fp_g18), lastRead);
        }

        free(message);
    }

    free(line);

    return read;
}

