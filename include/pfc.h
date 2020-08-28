#ifndef pfc_h
#define pfc_h

#define TEMPO 1 //1 secondo
#define DEFAULT_PROTOCOL 0
#define RAGGIO_TERRA_METRI 6367863
#define NMEA_FORMAT "$GPGLL"
#define MAX_SPEED_DIGITS 5
#define MAX_LINE_LENGTH 150
#define MAX_G18_FILE_LENGTH_DIGITS 6 /*Massimo numero di cifre della lunghezza in bytes del file g18.txt*/

#include "utility.h"

void setSignalStatus(int signalReceived, enum boolean *PFC_sigUsr, enum boolean *PFC_sigRestart);
void addLastRead(int fd_last_read, long current_position);
void changePointerPosition(FILE *fp_g18, int last_read);
int setPreviousGeographicCoordinates(FILE *fp_g18, double *previousLatitude, double *previousLongitude);

double changeSpeed(double speed);
ssize_t readCorrectLine(char *buffer, size_t bufferLength, FILE *fp);
void getGeographicCoordinates(char* line, double* latitude, double* longitude);
double getDistance(double latitude, double longitude, double previousLatitude, double previousLongitude);
double degreeToRadian(double degree);
double getVelocity(double space, int time);

int exe(int fd_pfcToTransducers, FILE *fp_g18, int last_read, double *previousLatitude, double *previousLongitude, enum boolean *sigUsr, enum boolean *sigRestart);

#endif
