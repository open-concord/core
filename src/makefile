SHELL = /bin/sh

# flag for /include dir
IN_DIR = -I../inc/ -I/usr/include/boost
# c/cpp standard
CC = g++
# compile-time flags
CFLAGS = -c -g -Wall -fPIC $(IN_DIR)
# lib flags
LFLAGS = -lssl -lcryptopp -lcrypt -lcrypto -pthread -lboost_system -lboost_filesystem -pthread

# object file sub-dir of src
BIN = _bin/

# .so args
SOFLAGS = -shared -Wl,-soname,libconcord.so

# .dll args
DLLFLAGS = -shared

# objects
OBJ = $(BIN)lockmsg.o \
	$(BIN)AES.o \
	$(BIN)RSA.o \
	$(BIN)DSA.o \
	$(BIN)hash.o \
	$(BIN)miner.o \
	$(BIN)time.o \
	$(BIN)tree.o \
	$(BIN)filetree.o \
	$(BIN)nlogic.o \
	$(BIN)nconn.o \
	$(BIN)nnode.o \
	$(BIN)chainsearch.o \
	$(BIN)chainmisc.o \
	$(BIN)b64.o \
	$(BIN)rw.o

default: main

main: $(BIN)main.o $(OBJ)
	@echo "NOW COMPILING MAIN EXECUTABLE"
	$(CC) $(CFLAGS) $(BIN)main.o $(OBJ) $(LFLAGS) -o $@

install: install_unix

install_unix: unix
	@echo "INSTALLING FOR UNIX"
	sudo cp libconcord.so /usr/lib/libconcord.so
	sudo ldconfig
	sudo mkdir /usr/include/concord
	sudo cp ../inc/* /usr/include/concord

# ex. `make unix`
unix: $(OBJ)
	@echo "NOW COMPILING | MAIN SHARED LIBRARY"
	$(CC) $(SOFLAGS) $(OBJ) $(LFLAGS) -o libconcord.so

# ex. `make win`
win: $(OBJ)
	@echo "NOW COMPILING | MAIN DLL"
	$(CC) $(DLLFLAGS) $(OBJ) $(LFLAGS) -o concord.dll

# functional stuff
$(BIN)main.o: main.cpp
	$(CC) $(CFLAGS) $< -o $@

$(BIN)b64.o: b64/b64.cpp ../inc/b64.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)hash.o: hash/hash.cpp ../inc/hash.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)miner.o: miner/miner.cpp ../inc/miner.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)time.o: time/timewizard.cpp ../inc/timewizard.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)filetree.o: tree/filetree.cpp ../inc/tree.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)tree.o: tree/tree.cpp ../inc/tree.h
	$(CC) $(CFLAGS) $< -o $@

# chain utils
$(BIN)chainsearch.o: chain_utils/chain_search.cpp ../inc/chain_utils.h
	$(CC) $(CFLAGS) $< -o $@
$(BIN)chainmisc.o: chain_utils/chain_misc.cpp ../inc/chain_utils.h
	$(CC) $(CFLAGS) $< -o $@

# nodes
$(BIN)nlogic.o: node/logic.cpp ../inc/node.h
	$(CC) $(CFLAGS) $< -o $@
$(BIN)nconn.o: node/conn.cpp ../inc/node.h
	$(CC) $(CFLAGS) $< -o $@
$(BIN)nnode.o: node/node.cpp ../inc/node.h
	$(CC) $(CFLAGS) $< -o $@

# rw
$(BIN)rw.o: rw/rw.cpp ../inc/rw.h
	$(CC) $(CFLAGS) $< -o $@

# crypt
$(BIN)lockmsg.o: crypt++/lockmsg.cpp ../inc/crypt++.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)DSA.o: crypt++/DSA.cpp ../inc/crypt++.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)RSA.o: crypt++/RSA.cpp ../inc/crypt++.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)AES.o: crypt++/AES.cpp ../inc/crypt++.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	cd $(BIN); rm -rf *o main
	rm libconcord.so