#ifndef utility_h
#define utility_h

/*
 * "[ $(ls __PATHDIR__ | wc -l) -gt 0 ] && rm __PATHDIR__* || :"
 */
#define remove_files_from(PATHDIR) "[ $(ls " PATHDIR " | wc -l) -gt 0 ] && rm " PATHDIR "* || :"
#define concat(DEST, S)            DEST S

/*
 * array_length considera anche il carattere di fine stringa
 * perciò dobbiamo sottrarre 1 per il calcolo della lunghezza
 * effettiva
 */
#define array_length(ARRAY)        (sizeof(ARRAY)/sizeof(ARRAY[0]))
#define string_length(S)           (array_length(S)-1)

enum boolean {FALSE, TRUE};

int createChild(int (*execv_function)(const char*, char* const*), char *filename, char **argv);
int numberOfDigits(int value);
void removeLastChar(char *string);
double inverse(double number);
void tokenize(char *string, char *separator, int tokenNumber, ...);

#endif
