all: main pfcDisconnectedSwitch generatoreFallimenti pfc1 pfc2 pfc3 transducer wes

main : main.o utility.o
    gcc -o main main.o utility.h utility.o

main.o : main.c utility.h
    gcc -c main.c


pfcDisconnectedSwitch: pfcDisconnectedSwitch.o utility.o
    gcc -o pfcDisconnectedSwitch pfcDisconnectedSwitch.o utility.o

pfcDisconnectedSwitch.o: pfcDisconnectedSwitch.c pfcDisconnectedSwitch.h
    gcc -c pfcDisconnectedSwitch.c


generatoreFallimenti: generatoreFallimenti.o
    gcc -o generatoreFallimenti generatoreFallimenti.o

generatoreFallimenti.o: generatoreFallimenti.c generatoreFallimenti.h
    gcc -c generatoreFallimenti.c


pfc1: pfc1.o pfc.o utility.o
    gcc -o pfc1 pfc1.o pfc.o utility.o

pfc1.o: pfc1.c pfc.h utility.h
    gcc -c pfc1.c

pfc2: pfc2.o pfc.o utility.o
    gcc -o pfc2 pfc2.o pfc.o utility.o

pfc2.o: pfc2.c pfc.h utility.h
    gcc -c pfc2.c

pfc3: pfc3.o pfc.o utility.o
    gcc -o pfc3 pfc3.o pfc.o utility.o

pfc3.o: pfc3.c pfc.h utility.h
    gcc -c pfc3.c

pfc.o: pfc.c pfc.h utility.h
    gcc -c pfc.c


transducer: transducer.o utility.o
    gcc -o transducer transducer.o utility.o

transducer.o: transducer.c transducer.h
    gcc -c transducer.c


wes: wes.o utility.o
    gcc -o wes wes.o

wes.o: wes.c wes.h
    gcc -c wes.c

utility.o : utility.c utility.h
    gcc -c utility.c




