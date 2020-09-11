#!/bin/bash
INPUT="../doc/G18.txt"

make
cd bin
./main $INPUT 
