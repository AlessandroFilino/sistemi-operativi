#ifndef generatoreFallimenti_h
#define generatoreFallimenti_h

#define PROB_SIGSTOP (1/100)
#define PROB_SIGINT (1/10000)
#define PROB_SIGCONT (1/10)
#define PROB_SIGUSR1 (1/10)
#define PROB_TOT 10000

typedef struct {
    unsigned value : 4;
} u_4;

u_4 calcoloProb(void);
void readNewFd(char *buffer_newPid, int *pfcId, int *newPid);

#endif
