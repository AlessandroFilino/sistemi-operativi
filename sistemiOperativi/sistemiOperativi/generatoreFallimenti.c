//
//  generatoreFallimenti.c
//  sistemiOperativi
//
//  Created by Alessandro Filino on 18/06/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//

#include "generatoreFallimenti.h"
#define PROB_SIGSTOP 100;
#define PROB_SIGINT 10000;
#define PROB_SIGCONT 10;
#define PROB_SIGUSR1 10;


int main() {
    
    FILE *failures;
    failures = fopen("failures.log", "w");
    
    sleep(1);
    
    int pfc = (rand() % 3) + 1;
    unsigned char fallimenti = calcoloProb();
    
    if (fallimenti & 0){
        
    }
    
    if (fallimenti & 2){
        
    }
    
    if (fallimenti & 4) {
        
    }
    
    if (fallimenti & 8){
        
    }
 
}

unsigned char calcoloProb(void){
    unsigned char prob;
    
    return 0;
}
