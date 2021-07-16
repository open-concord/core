##### ReWrite as per GNU conventions on Makefiles ####

SHELL = /bin/sh
# fix any issues w/ suffixes
.SUFFIXES:
.SUFFIXES: .cpp .o

# calls main rule (allowing us to return the exe to the root) [possibly take command line input for future releases??]
default: ../main

# flag for /include dir
IN_DIR = -I../inc/ -I/usr/include/boost
# c/cpp standard
CC = g++
# compile-time flags
CFLAGS = -g -Wall $(IN_DIR)
# lib flags
LFLAGS = -lssl -lcrypt -lcrypto -pthread -lboost_system -lboost_filesystem -pthread

# object file sub-dir of src
BIN = _bin/

# objects
OBJ = $(BIN)hexstr.o \
	$(BIN)rawcrypt.o \
	$(BIN)highcrypt.o \
	$(BIN)hexcrypt.o \
	$(BIN)hash.o \
	$(BIN)ledger.o \
	$(BIN)miner.o \
	$(BIN)time.o \
	$(BIN)tree.o \
	$(BIN)filetree.o \
	$(BIN)nlogic.o \
	$(BIN)nconn.o \
	$(BIN)nnode.o \
	$(BIN)b64.o \
	$(BIN)rw.o \
	$(BIN)main.o

# returns executable to root

../main: $(OBJ)
	@echo "NOW COMPILING MAIN EXECUTABLE"
	$(CC) $(CFLAGS) $(OBJ) $(LFLAGS) -o $@

$(BIN)main.o: main.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)hexstr.o: hexstr/hexstr.cpp ../inc/hexstr.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)rawcrypt.o: crypt/rawcrypt.cpp ../inc/crypt.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)highcrypt.o: crypt/highcrypt.cpp ../inc/crypt.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)hexcrypt.o: crypt/hexcrypt.cpp ../inc/crypt.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)hash.o: hash/hash.cpp ../inc/hash.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)ledger.o: ledger/ledger.cpp ../inc/ledger.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)miner.o: miner/miner.cpp ../inc/miner.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)time.o: time/timewizard.cpp ../inc/timewizard.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)filetree.o: tree/filetree.cpp ../inc/tree.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)tree.o: tree/tree.cpp ../inc/tree.h
	$(CC) -c $(CFLAGS) $< -o $@

# node stuff
$(BIN)nlogic.o: node/logic.cpp ../inc/node.h
	$(CC) -c $(CFLAGS) $< -o $@
$(BIN)nconn.o: node/conn.cpp ../inc/node.h
	$(CC) -c $(CFLAGS) $< -o $@
$(BIN)nnode.o: node/node.cpp ../inc/node.h
	$(CC) -c $(CFLAGS) $< -o $@

# rw
$(BIN)rw.o: rw/rw.cpp ../inc/rw.h
	$(CC) -c $(CFLAGS) $< -o $@
$(BIN)b64.o: rw/base64.cpp ../inc/rw.h
	$(CC) -c $(CFLAGS) $< -o $@
clean:
	cd $(BIN); rm -rf *o main
