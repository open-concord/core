##### ReWrite as per GNU conventions on Makefiles ####

SHELL = /bin/sh
# fix any issues w/ suffixes
.SUFFIXES:
.SUFFIXES: .cpp .o

# calls main rule (allowing us to return the exe to the root) [possibly take command line input for future releases??]
default: ../main

# flag for /include dir
IN_DIR = -I../include/
# c/cpp standard
CC = g++
# compile-time flags
CFLAGS = -g -Wall $(IN_DIR)
# lib flags
LFLAGS = -lssl -lcrypt -lcrypto

# object file sub-dir of src
BIN = _bin/

# objects
OBJ = $(BIN)hash.o \
	$(BIN)ledger.o \
	$(BIN)miner.o \
	$(BIN)net.o \
	$(BIN)time.o \
	$(BIN)tree.o \
	$(BIN)main.o

# returns executable to root
../main: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LFLAGS) -o $@

$(BIN)main.o: main.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)hash.o: hash/hash.cpp ../inc/hash.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)ledger.o: ledger/ledger.cpp ../inc/ledger.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)miner.o: miner/miner.cpp ../inc/miner.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)net.o: net/server.cpp net/client.cpp ../inc/net.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)time.o: time/timewizard.cpp ../inc/timewizard.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)tree.o: tree/tree.cpp ../inc/tree.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	cd $(BIN); rm -rf *o main