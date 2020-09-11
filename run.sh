#!/bin/bash
INPUT="../doc/G18-test2.txt"

make
cd bin
./main $INPUT 
