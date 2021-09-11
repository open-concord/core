SHELL = /bin/sh

# flag for /include dir
IN_DIR = -I../inc/ -I/usr/include/boost
# c/cpp standard
CC = g++ -std=c++20
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
	$(BIN)ttime.o \
	$(BIN)tree.o \
	$(BIN)tbenc.o \
	$(BIN)tmisc.o \
	$(BIN)b64.o \
	$(BIN)hexstr.o \
	$(BIN)strutil.o \
	$(BIN)smisc.o \
	$(BIN)server.o

#	$(BIN)nlogic.o \
	$(BIN)nconn.o \
	$(BIN)nnode.o

default: unix

install: install_unix

install_unix: unix
	@echo "INSTALLING FOR UNIX"
	sudo cp libconcord.so /usr/lib/libconcord.so
	sudo ldconfig
	sudo mkdir -p /usr/include/concord
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

$(BIN)b64.o: strenc/b64.cpp ../inc/strenc.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)hexstr.o: strenc/hexstr.cpp ../inc/strenc.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)strutil.o: strenc/strutil.cpp ../inc/strenc.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)hash.o: hash/hash.cpp ../inc/hash.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)miner.o: miner/miner.cpp ../inc/miner.h
	$(CC) $(CFLAGS) $< -o $@

$(BIN)tree.o: tree/tree.cpp ../inc/tree.h
	$(CC) $(CFLAGS) $< -o $@

# chain utils
#$(BIN)tsearch.o: tree/search.cpp ../inc/tree.h
#	$(CC) $(CFLAGS) $< -o $@
$(BIN)tmisc.o: tree/chain_misc.cpp ../inc/tree.h
	$(CC) $(CFLAGS) $< -o $@
$(BIN)ttime.o: tree/time_enc.cpp ../inc/tree.h
	$(CC) $(CFLAGS) $< -o $@
$(BIN)tbenc.o: tree/block_enc.cpp ../inc/tree.h
	$(CC) $(CFLAGS) $< -o $@
	
#server
$(BIN)server.o: server/server.cpp ../inc/server.h
$(BIN)smisc.o: server/claf_misc.cpp ../inc/server.h

# nodes
#$(BIN)nlogic.o: node/logic.cpp ../inc/node.h
#	$(CC) $(CFLAGS) $< -o $@
#$(BIN)nconn.o: node/conn.cpp ../inc/node.h
#	$(CC) $(CFLAGS) $< -o $@
#$(BIN)nnode.o: node/node.cpp ../inc/node.h
#	$(CC) $(CFLAGS) $< -o $@

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
	rm -f libconcord.so