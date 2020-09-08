BIN_DIR=bin
OBJ_DIR=bin
SRC_DIR=src
HEADER_DIR=include
LOG_DIR=log
TEMP_DIR=temp
VERBOSE=@

all: create-dir main pfcDisconnectedSwitch generatoreFallimenti pfc1 pfc2 pfc3 transducers wes clean-objects

create-dir :
	$(VERBOSE) mkdir -p $(LOG_DIR) $(TEMP_DIR) $(BIN_DIR) $(OBJ_DIR)

main :  main.o utility.o
	$(VERBOSE) gcc -o $(BIN_DIR)/main $(OBJ_DIR)/main.o $(OBJ_DIR)/utility.o

main.o : $(SRC_DIR)/main.c $(HEADER_DIR)/main.h $(HEADER_DIR)/utility.h $(HEADER_DIR)/path.h $(HEADER_DIR)/messages.h
	$(VERBOSE) gcc -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o


pfcDisconnectedSwitch : pfcDisconnectedSwitch.o utility.o
	$(VERBOSE) gcc -o $(BIN_DIR)/pfcDisconnectedSwitch $(OBJ_DIR)/pfcDisconnectedSwitch.o $(OBJ_DIR)/utility.o

pfcDisconnectedSwitch.o : $(SRC_DIR)/pfcDisconnectedSwitch.c $(HEADER_DIR)/pfcDisconnectedSwitch.h $(HEADER_DIR)/utility.h $(HEADER_DIR)/path.h $(HEADER_DIR)/messages.h
	$(VERBOSE) gcc -c $(SRC_DIR)/pfcDisconnectedSwitch.c -o $(OBJ_DIR)/pfcDisconnectedSwitch.o


generatoreFallimenti : generatoreFallimenti.o utility.o
	$(VERBOSE) gcc -o $(BIN_DIR)/generatoreFallimenti $(OBJ_DIR)/generatoreFallimenti.o $(OBJ_DIR)/utility.o

generatoreFallimenti.o : $(SRC_DIR)/generatoreFallimenti.c $(HEADER_DIR)/generatoreFallimenti.h $(HEADER_DIR)/utility.h $(HEADER_DIR)/path.h $(HEADER_DIR)/messages.h
	$(VERBOSE) gcc -c $(SRC_DIR)/generatoreFallimenti.c -o $(OBJ_DIR)/generatoreFallimenti.o


pfc1 : pfc1.o pfc.o utility.o
	$(VERBOSE) gcc -o $(BIN_DIR)/pfc1 $(OBJ_DIR)/pfc1.o $(OBJ_DIR)/pfc.o $(OBJ_DIR)/utility.o

pfc1.o : $(SRC_DIR)/pfc1.c $(HEADER_DIR)/pfc.h $(HEADER_DIR)/utility.h $(HEADER_DIR)/path.h $(HEADER_DIR)/messages.h
	$(VERBOSE) gcc -c $(SRC_DIR)/pfc1.c -o $(OBJ_DIR)/pfc1.o

pfc2 : pfc2.o pfc.o utility.o
	$(VERBOSE) gcc -o $(BIN_DIR)/pfc2 $(OBJ_DIR)/pfc2.o $(OBJ_DIR)/pfc.o $(OBJ_DIR)/utility.o

pfc2.o : $(SRC_DIR)/pfc2.c $(HEADER_DIR)/pfc.h $(HEADER_DIR)/utility.h $(HEADER_DIR)/path.h $(HEADER_DIR)/messages.h
	$(VERBOSE) gcc -c $(SRC_DIR)/pfc2.c -o $(OBJ_DIR)/pfc2.o

pfc3 : pfc3.o pfc.o utility.o
	$(VERBOSE) gcc -o $(BIN_DIR)/pfc3 $(OBJ_DIR)/pfc3.o $(OBJ_DIR)/pfc.o $(OBJ_DIR)/utility.o

pfc3.o : $(SRC_DIR)/pfc3.c $(HEADER_DIR)/pfc.h $(HEADER_DIR)/utility.h $(HEADER_DIR)/path.h $(HEADER_DIR)/messages.h
	$(VERBOSE) gcc -c $(SRC_DIR)/pfc3.c -o $(OBJ_DIR)/pfc3.o

pfc.o : $(SRC_DIR)/pfc.c $(HEADER_DIR)/pfc.h $(HEADER_DIR)/utility.h
	$(VERBOSE) gcc -c $(SRC_DIR)/pfc.c -o $(OBJ_DIR)/pfc.o


transducers : transducers.o utility.o
	$(VERBOSE) gcc -o $(BIN_DIR)/transducers $(OBJ_DIR)/transducers.o $(OBJ_DIR)/utility.o

transducers.o : $(SRC_DIR)/transducers.c $(HEADER_DIR)/transducers.h $(HEADER_DIR)/utility.h $(HEADER_DIR)/path.h $(HEADER_DIR)/messages.h
	$(VERBOSE) gcc -c $(SRC_DIR)/transducers.c -o $(OBJ_DIR)/transducers.o


wes: wes.o utility.o
	$(VERBOSE) gcc -o $(BIN_DIR)/wes $(OBJ_DIR)/wes.o $(OBJ_DIR)/utility.o

wes.o: $(SRC_DIR)/wes.c $(HEADER_DIR)/wes.h $(HEADER_DIR)/utility.h $(HEADER_DIR)/path.h $(HEADER_DIR)/messages.h
	$(VERBOSE) gcc -c $(SRC_DIR)/wes.c -o $(OBJ_DIR)/wes.o

utility.o : $(SRC_DIR)/utility.c $(HEADER_DIR)/utility.h
	$(VERBOSE) gcc -c $(SRC_DIR)/utility.c -o $(OBJ_DIR)/utility.o


clean : 
	$(VERBOSE) rm $(BIN_DIR)/*

clean-objects : 
	$(VERBOSE) rm $(OBJ_DIR)/*.o




