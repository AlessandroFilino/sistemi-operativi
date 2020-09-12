#!/bin/bash
INPUT="../doc/G18-test2.txt"

make -f makefile.make
cd bin
./main $INPUT 
