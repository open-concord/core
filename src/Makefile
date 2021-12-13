SHELL = /bin/sh

# includes + libs
# unix file system
UUSR = -I/usr/include/ -L/usr/lib/
# local directory
LDIR = -I../lib/inc/ -I../inc/ -L../lib/

# cpp standard
CC = g++ -std=c++20

# compile-time flags
CFLAGS = -c -g -Wall -fPIC
# lib flags
LFLAGS = -lcryptopp -pthread -luttu

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
	$(BIN)b64.o \
	$(BIN)hexstr.o \
	$(BIN)strutil.o \
	$(BIN)smisc.o \
	$(BIN)sbranch_ctx.o \
	$(BIN)server.o \
  $(BIN)nlogic.o \
	$(BIN)nconn.o \
	$(BIN)nnode.o

default: static_unix

install_unix: unix
	@echo "INSTALLING FOR UNIX"
	sudo cp libconcord.so /usr/lib/libconcord.so
	sudo ldconfig
	sudo mkdir -p /usr/include/concord
	sudo cp ../inc/* /usr/include/concord

# linking rules
shared_windows: $(OBJ)
	@echo "NOW BUILDING (WINDOWS) | DLL"
	$(CC) $(DLLFLAGS) $(OBJ) $(LFLAGS) -o core.dll

shared_unix: $(OBJ)
	@echo "NOW BUILDING (UNIX) | SHARED OBJECT"
	$(CC) $(SOFLAGS) $(OBJ) $(LFLAGS) -o libcore.so

static_windows: $(OBJ)
	@echo "NOW LINKING (WINDOWS) | ARCHIVE"

static_unix: $(OBJ)
	@echo "NOW LINKING (UNIX) | ARCHIVE"
	ar ru libcore.a $^ #$(UUSR) $(LLIB)
	ranlib libcore.a
	mv libcore.a ../exe/
	cp -r ../inc ../exe/inc

# build rules
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
$(BIN)ttime.o: tree/time_enc.cpp ../inc/tree.h
	$(CC) $(CFLAGS) $< -o $@
$(BIN)tbenc.o: tree/block_enc.cpp ../inc/tree.h
	$(CC) $(CFLAGS) $< -o $@

# server
$(BIN)server.o: server/server.cpp ../inc/server.h
	$(CC) $(CFLAGS) $< -o $@
$(BIN)smisc.o: server/claf_misc.cpp ../inc/server.h
	$(CC) $(CFLAGS) $< -o $@
$(BIN)sbranch_ctx.o: server/branch_ctx.cpp ../inc/server.h
	$(CC) $(CFLAGS) $< -o $@

# nodes
$(BIN)nlogic.o: node/logic.cpp ../inc/node.h
	$(CC) $(CFLAGS) $< -o $@ -I../lib/inc/uttu.hpp
$(BIN)nconn.o: node/conn.cpp ../inc/node.h
	$(CC) $(CFLAGS) $< -o $@ -I../lib/inc
$(BIN)nnode.o: node/node.cpp ../inc/node.h
	$(CC) $(CFLAGS) $< -o $@ -I../lib/inc

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
