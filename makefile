VERBOSE=@

all: create-dir main pfcDisconnectedSwitch generatoreFallimenti pfc1 pfc2 pfc3 transducers transducers-socket wes clean-objects

create-dir :
	$(VERBOSE) mkdir -p log temp bin

main : main.o utility.o utility-textfile.o
	$(VERBOSE) gcc -o bin/main bin/main.o bin/utility.o bin/utility-textfile.o

main.o : src/main.c include/main.h include/utility.h include/utility-textfile.h include/config.h include/messages.h
	$(VERBOSE) gcc -c src/main.c -o bin/main.o


pfcDisconnectedSwitch : pfcDisconnectedSwitch.o utility.o utility-textfile.o utility-ipc.o
	$(VERBOSE) gcc -o bin/pfcDisconnectedSwitch bin/pfcDisconnectedSwitch.o bin/utility.o bin/utility-textfile.o bin/utility-ipc.o

pfcDisconnectedSwitch.o : src/pfcDisconnectedSwitch.c include/pfcDisconnectedSwitch.h include/utility.h include/utility-textfile.h include/utility-ipc.h include/config.h include/messages.h
	$(VERBOSE) gcc -c src/pfcDisconnectedSwitch.c -o bin/pfcDisconnectedSwitch.o


generatoreFallimenti : generatoreFallimenti.o utility.o utility-textfile.o utility-ipc.o
	$(VERBOSE) gcc -o bin/generatoreFallimenti bin/generatoreFallimenti.o bin/utility.o bin/utility-textfile.o bin/utility-ipc.o

generatoreFallimenti.o : src/generatoreFallimenti.c include/generatoreFallimenti.h include/utility.h include/utility-textfile.h include/utility-ipc.h include/config.h include/messages.h
	$(VERBOSE) gcc -c src/generatoreFallimenti.c -o bin/generatoreFallimenti.o


pfc1 : pfc1.o pfc.o utility.o utility-textfile.o utility-ipc.o utility-math.o
	$(VERBOSE) gcc -o bin/pfc1 bin/pfc1.o bin/pfc.o bin/utility.o bin/utility-textfile.o bin/utility-ipc.o bin/utility-math.o -lm

pfc1.o : src/pfc1.c include/pfc.h include/utility.h include/utility-textfile.h include/utility-ipc.h include/config.h include/messages.h
	$(VERBOSE) gcc -c src/pfc1.c -o bin/pfc1.o

pfc2 : pfc2.o pfc.o utility.o utility-textfile.o utility-ipc.o utility-math.o
	$(VERBOSE) gcc -o bin/pfc2 bin/pfc2.o bin/pfc.o bin/utility.o bin/utility-textfile.o bin/utility-ipc.o bin/utility-math.o -lm

pfc2.o : src/pfc2.c include/pfc.h include/utility.h include/utility-textfile.h include/utility-ipc.h include/config.h include/messages.h
	$(VERBOSE) gcc -c src/pfc2.c -o bin/pfc2.o

pfc3 : pfc3.o pfc.o utility.o utility-textfile.o utility-math.o
	$(VERBOSE) gcc -o bin/pfc3 bin/pfc3.o bin/pfc.o bin/utility.o bin/utility-textfile.o bin/utility-math.o -lm

pfc3.o : src/pfc3.c include/pfc.h include/utility.h include/utility-textfile.h include/config.h include/messages.h
	$(VERBOSE) gcc -c src/pfc3.c -o bin/pfc3.o

pfc.o : src/pfc.c include/pfc.h include/utility.h include/utility-math.h include/config.h
	$(VERBOSE) gcc -c src/pfc.c -o bin/pfc.o


transducers : transducers.o utility.o utility-textfile.o utility-ipc.o
	$(VERBOSE) gcc -o bin/transducers bin/transducers.o bin/utility.o bin/utility-textfile.o bin/utility-ipc.o

transducers.o : src/transducers.c include/transducers.h include/utility.h include/utility-textfile.h include/utility-ipc.h include/config.h include/messages.h
	$(VERBOSE) gcc -c src/transducers.c -o bin/transducers.o


transducers-socket : transducers-socket.o utility.o utility-textfile.o utility-ipc.o
	$(VERBOSE) gcc -o bin/transducers-socket bin/transducers-socket.o bin/utility.o bin/utility-textfile.o bin/utility-ipc.o

transducers-socket.o : src/transducers-socket.c include/transducers-socket.h include/utility.h include/utility-textfile.h include/utility-ipc.h include/config.h include/messages.h
	$(VERBOSE) gcc -c src/transducers-socket.c -o bin/transducers-socket.o


wes: wes.o utility.o utility-textfile.o utility-ipc.o
	$(VERBOSE) gcc -o bin/wes bin/wes.o bin/utility.o bin/utility-textfile.o bin/utility-ipc.o

wes.o: src/wes.c include/wes.h include/utility.h include/utility-textfile.h include/utility-ipc.h include/config.h include/messages.h
	$(VERBOSE) gcc -c src/wes.c -o bin/wes.o


utility.o : src/utility.c include/utility.h
	$(VERBOSE) gcc -c src/utility.c -o bin/utility.o

utility-ipc.o : src/utility-ipc.c include/utility.h include/utility-ipc.h
	$(VERBOSE) gcc -c src/utility-ipc.c -o bin/utility-ipc.o

utility-math.o : src/utility-math.c include/utility-math.h
	$(VERBOSE) gcc -c src/utility-math.c -o bin/utility-math.o

utility-textfile.o : src/utility-textfile.c include/utility-textfile.h
	$(VERBOSE) gcc -c src/utility-textfile.c -o bin/utility-textfile.o

clean : 
	$(VERBOSE) rm bin/*

clean-objects : 
	$(VERBOSE) rm bin/*.o




