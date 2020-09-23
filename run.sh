#!/bin/bash
G18="../doc/G18.txt"
G18_1="../doc/G18-test.txt"
G18_2="../doc/G18-test2.txt"
G18_3="../doc/G18-test3.txt"

make -f makefile.make
cd bin

# In base al numero passato in input, viene eseguito il programma
# leggendo da uno dei file di testo sopra indicati.

if [ $1 -eq 0 ]
then
	./main $G18 
elif [ $1 -eq 1 ]
then
	./main $G18_1 
elif [ $1 -eq 2 ]
then
	./main $G18_2 
elif [ $1 -eq 3 ]
then
	./main $G18_3 
else
	echo ERROR: input must be between 0 and 3
fi




