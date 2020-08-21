all: create-dir main pfcDisconnectedSwitch generatoreFallimenti pfc1 pfc2 pfc3 transducers wes clean-objects

create-dir :
	mkdir -p log temp bin

main : main.o utility.o
	gcc -o bin/main main.o utility.o

main.o : src/main.c include/main.h include/utility.h
	gcc -c src/main.c


pfcDisconnectedSwitch : pfcDisconnectedSwitch.o utility.o
	gcc -o bin/pfcDisconnectedSwitch pfcDisconnectedSwitch.o utility.o

pfcDisconnectedSwitch.o : src/pfcDisconnectedSwitch.c include/pfcDisconnectedSwitch.h
	gcc -c src/pfcDisconnectedSwitch.c


generatoreFallimenti : generatoreFallimenti.o
	gcc -o bin/generatoreFallimenti generatoreFallimenti.o

generatoreFallimenti.o : src/generatoreFallimenti.c include/generatoreFallimenti.h
	gcc -c src/generatoreFallimenti.c


pfc1 : pfc1.o pfc.o utility.o
	gcc -o bin/pfc1 pfc1.o pfc.o utility.o

pfc1.o : src/pfc1.c include/pfc.h include/utility.h
	gcc -c src/pfc1.c

pfc2 : pfc2.o pfc.o utility.o
	gcc -o bin/pfc2 pfc2.o pfc.o utility.o

pfc2.o : src/pfc2.c include/pfc.h include/utility.h
	gcc -c src/pfc2.c

pfc3 : pfc3.o pfc.o utility.o
	gcc -o bin/pfc3 pfc3.o pfc.o utility.o

pfc3.o : src/pfc3.c include/pfc.h include/utility.h
	gcc -c src/pfc3.c

pfc.o : src/pfc.c include/pfc.h include/utility.h
	gcc -c src/pfc.c


transducers : transducers.o utility.o
	gcc -o bin/transducers transducers.o utility.o

transducers.o : src/transducers.c include/transducers.h
	gcc -c src/transducers.c


wes: wes.o utility.o
	gcc -o bin/wes wes.o utility.o

wes.o: src/wes.c include/wes.h
	gcc -c src/wes.c

utility.o : src/utility.c include/utility.h
	gcc -c src/utility.c


clean : 
	rm bin/*

clean-objects : 
	rm *.o




