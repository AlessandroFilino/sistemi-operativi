#ifndef utility_h
#define utility_h

#include <stdio.h> /* Per il tipo "FILE"*/
#include <sys/socket.h>
#include <sys/un.h>

/*
 * #define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))
 *
 * ARRAY_LENGTH considera anche il carattere di fine stringa
 * perciò dobbiamo sottrarre 1 per il calcolo della lunghezza
 * effettiva
 * #define STRLEN(s)           (ARRAY_LENGTH(s)-1)
 */

#define CONCAT(dest, s)     dest s

/*
 * "[ $(ls __PATHDIR__ | wc -l) -gt 0 ] && rm __PATHDIR__* || :"
 * TODO: controllare se l'ultimo carattere di PATHDIR è '/' e, in caso mancasse, inserirlo?
 */
#define REMOVE_FILES_FROM(PATHDIR) "[ $(ls " PATHDIR " | wc -l) -gt 0 ] && rm " PATHDIR "* || :"

enum boolean {FALSE, TRUE};

FILE *open_file(const char* filename, const char* mode);
int readLine(int fd, char *buffer, char delimiter);
int createChild(int (*execv_function)(const char*, char* const*), char *filename, char **argv);
int connectPipe(char *pipename, int mode);
int connectSocket(int clientFd, const struct sockaddr* serverSockAddrPtr, socklen_t serverLen);
int digits_number(int number);
enum boolean tokenize(char *string, char *separator, int tokenNumber, char *buffer[]);

void intToString(char *buffer, int buffer_size, int number);
int doubleToString(char *buffer, int buffer_size, double number);

#endif
