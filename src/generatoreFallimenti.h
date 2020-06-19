//
//  generatoreFallimenti.h
//  sistemiOperativi
//
//  Created by Alessandro Filino on 18/06/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//

#ifndef generatoreFallimenti_h
#define generatoreFallimenti_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define PROB_SIGSTOP 1/100;
#define PROB_SIGINT 1/10000;
#define PROB_SIGCONT 1/10;
#define PROB_SIGUSR1 1/10;
#define PROB_TOT 10000

unsigned char calcoloProb(void);

#endif /* generatoreFallimenti_h */
