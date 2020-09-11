#!/bin/bash
INPUT="../doc/G18-test.txt"

make
cd bin
./main $INPUT 
