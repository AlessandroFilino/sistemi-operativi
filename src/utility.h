#ifndef utility_h
#define utility_h

#include <stdio.h> /* Per il tipo "FILE"*/
#define TRUE 1
#define FALSE 0

FILE *open_file(const char* filename, const char* mode);
int readLine(int fd, char *buffer);
int createChild(int (*execv_function)(const char*, char* const*), char *filename, char **argv);
int digits_number(int number);
void intToString(char *buffer, int buffer_size, int number);
int doubleToString(char *buffer, int buffer_size, double number);

#endif
