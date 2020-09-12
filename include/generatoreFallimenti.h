#ifndef generatoreFallimenti_h
#define generatoreFallimenti_h

#define PROB_SIGSTOP (1.0/100)
//#define PROB_SIGSTOP (1.0/10)
#define PROB_SIGINT  (1.0/10000)
//#define PROB_SIGINT (1.0/10)
#define PROB_SIGCONT (1.0/10)
#define PROB_SIGUSR1 (1.0/10)

#define SIGSTOP_INDEX 1u
#define SIGINT_INDEX  2u
#define SIGCONT_INDEX 4u
#define SIGUSR1_INDEX 8u

void writeLog(FILE *log, char *string, int pfc);
unsigned char setProb(void);
enum boolean checkProb(double prob);
void readNewFd(char *buffer_newPid, int *pfcId, int *newPid);

#endif
