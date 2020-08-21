#ifndef generatoreFallimenti_h
#define generatoreFallimenti_h

#define PROB_SIGSTOP 1/100
#define PROB_SIGINT 1/10000
#define PROB_SIGCONT 1/10
#define PROB_SIGUSR1 1/10
#define PROB_TOT 10000

unsigned char calcoloProb(void);

#endif
