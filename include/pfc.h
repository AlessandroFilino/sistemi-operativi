#ifndef pfc_h
#define pfc_h

#define TEMPO 1 //1 secondo espresso in ore
#define RAGGIO_TERRA_METRI 6367863
#define NMEA_FORMAT "$GPGLL"
#define MAX_SPEED_DIGITS 5
#define MAX_LINE_LENGTH 150
#define SEPARATOR_TOKENIZER ","
#define MAX_G18_FILE_LENGTH_DIGITS 10 /*Massimo numero di cifre della lunghezza in bytes del file g18.txt*/

#include "utility.h"

void setSignalStatus(int signalReceived, enum boolean *PFC_sigUsr, enum boolean *PFC_sigRestart);
void addLastRead(long current_position, FILE *lastRead);
void changePointerPosition(FILE *fp_g18, FILE *lastRead);
int setPreviousGeographicCoordinates(FILE *fp_g18, double *previousLatitude, double *previousLongitude);
enum boolean checkCorrectPosition(FILE *fp_g18, FILE *lastRead);
 
double changeSpeed(double speed);
ssize_t readCorrectLine(char *buffer, size_t bufferLength, FILE *fp);
void getGeographicCoordinates(char* line, double* latitude, double* longitude);
double getDistance(double latitude, double longitude, double previousLatitude, double previousLongitude);
double degreeToRadian(double degree);
double getVelocity(double space, int time);

int exe(int fd_pfcToTransducers, FILE *fp_g18, FILE *lastRead, double *previousLatitude, double *previousLongitude, enum boolean *sigUsr, enum boolean *sigRestart);

#endif
