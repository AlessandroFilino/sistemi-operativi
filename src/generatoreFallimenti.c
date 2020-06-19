//
//  generatoreFallimenti.c
//  sistemiOperativi
//
//  Created by Alessandro Filino on 18/06/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//

#include "generatoreFallimenti.h"

int main(int argc, const char * argv[]) {

    srand(time(NULL));

    //int signals[4] = {SIGSTOP, SIGINT, SIGCONT, SIGUSR1};
    int pfcProcess[3] = {atoi(argv[1]), atoi(argv[2]), atoi(argv[3])};
    FILE *failures = fopen("failures.log", "w");
    
    sleep(1);
    
    int pfc = rand() % 3;
    unsigned char fallimenti = calcoloProb();
    
    if (fallimenti & 1){
        kill(pfcProcess[pfc], SIGSTOP);
    }
    
    if (fallimenti & 2){
        kill(pfcProcess[pfc], SIGINT);
    }
    
    if (fallimenti & 4) {
        kill(pfcProcess[pfc], SIGCONT);
    }
    
    if (fallimenti & 8){
        kill(pfcProcess[pfc], SIGUSR1);
    }
 
}

unsigned char calcoloProb(){
    unsigned char prob = 0;
    int value = rand();

    if((value % PROB_TOT) < (PROB_SIGSTOP * PROB_TOT)) {
        prob |= 1;
    }

    if((value % PROB_TOT) < (PROB_SIGINT * PROB_TOT)) {
        prob |= 2;
    }

    if((value % PROB_TOT) < (PROB_SIGCONT * PROB_TOT)) {
        prob |= 4;
    }

    if((value % PROB_TOT) < (PROB_SIGUSR1 * PROB_TOT)) {
        prob |= 8;
    }

    return prob;
}
