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
OBJ = $(BIN)b64.o \
	$(BIN)AES.o \
	$(BIN)RSA.o \
	$(BIN)DSA.o \
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
	$(BIN)aes.o \
	$(BIN)main.o

# returns executable to root

../main: $(OBJ)
	@echo "NOW COMPILING MAIN EXECUTABLE"
	$(CC) $(CFLAGS) $(OBJ) $(LFLAGS) -o $@

$(BIN)main.o: main.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)lockmsg.o: crypt++/lockmsg.cpp ../inc/crypt++.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)DSA.o: crypt++/DSA.cpp ../inc/crypt++.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)RSA.o: crypt++/RSA.cpp ../inc/crypt++.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)AES.o: crypt++/AES.cpp ../inc/crypt++.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN)b64.o: b64/b64.cpp ../inc/b64.h
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

# crypt
$(BIN)aes.o: crypt++/AES.cpp ../inc/crypt++
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	cd $(BIN); rm -rf *o main
