##### ReWrite as per GNU conventions on Makefiles ####

SHELL = /bin/sh
# fix any issues w/ suffixes
.SUFFIXES:
.SUFFIXES: .cpp .o

# calls main rule (allowing us to return the exe to the root) [possibly take command line input for future releases??]
default: ../testnode

# flag for /include dir
IN_DIR = -I../include/ -I/usr/include/boost
# c/cpp standard
CC = g++
# compile-time flags
CFLAGS = -g -Wall $(IN_DIR)
# lib flags
LFLAGS = -lssl -lcrypt -lcrypto -pthread

# object file sub-dir of src
BIN = _bin/

# objects
OBJ = $(BIN)node.o \
	$(BIN)main.o

# returns executable to root

../testnode: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LFLAGS) -o $@

$(BIN)main.o: main.cpp
	$(CC) -c $(CFLAGS) $< -o $@
$(BIN)node.o: ../../src/node/node.cpp ../../inc/node.h
	$(CC) -c $(CFLAGS) $< -o $@
clean:
	cd $(BIN); rm -rf *o main