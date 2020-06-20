//
//  pfcDisconnectedSwitch.h
//  sistemiOperativi
//
//  Created by Alessandro Filino on 26/05/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//

#ifndef pfcDisconnectedSwitch_h
#define pfcDisconnectedSwitch_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


char* intToString(int number);
int creaFiglio(int (*execv_function)(const char*, char* const*), char *filename, char *argv[]);
void childHandler(int);

#endif
