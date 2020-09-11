#ifndef generatoreFallimenti_h
#define generatoreFallimenti_h

//#define PROB_SIGSTOP (1.0/100)
#define PROB_SIGSTOP (1.0/50)
//#define PROB_SIGINT  (1.0/10000)
#define PROB_SIGINT  (1.0/100)
#define PROB_SIGCONT (1.0/10)
#define PROB_SIGUSR1 (1.0/10)

typedef struct {
    unsigned value : 4;
} u_4;

u_4 calcoloProb(void);
void readNewFd(char *buffer_newPid, int *pfcId, int *newPid);

#endif
