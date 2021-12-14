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


$(BIN)main.o: main.cpp
	$(CC) $(CFLAGS) $< -o $@

# string manipulation
$(BIN)b64.o: ../inc/strops.h
	$(CC) $(CFLAGS) strops/b64.cpp -o $@

$(BIN)hexstr.o: ../inc/strops.h
	$(CC) $(CFLAGS) strops/hexstr.cpp -o $@

$(BIN)strutil.o: ../inc/strops.h
	$(CC) $(CFLAGS) strops/strutil.cpp -o $@

$(BIN)hash.o: ../inc/strops.h
	$(CC) $(CFLAGS) strops/hash.cpp -o $@

$(BIN)miner.o: ../inc/miner.h
	$(CC) $(CFLAGS) miner/miner.cpp -o $@

$(BIN)tree.o: ../inc/tree.h
	$(CC) $(CFLAGS) tree/tree.cpp -o $@

# chain utils
$(BIN)ttime.o: ../inc/tree.h
	$(CC) $(CFLAGS) tree/time_enc.cpp -o $@
$(BIN)tbenc.o: ../inc/tree.h
	$(CC) $(CFLAGS) tree/block_enc.cpp -o $@

# server abstraction
$(BIN)server.o: ../inc/server.h
	$(CC) $(CFLAGS) server/server.cpp -o $@
$(BIN)smisc.o: ../inc/server.h
	$(CC) $(CFLAGS) server/claf_misc.cpp -o $@
$(BIN)sbranch_ctx.o: ../inc/server.h
	$(CC) $(CFLAGS) server/branch_ctx.cpp -o $@

# nodes
$(BIN)nlogic.o: ../inc/node.h ../lib/inc
	$(CC) $(CFLAGS) node/logic.cpp -o $@
$(BIN)nconn.o: ../inc/node.h ../lib/inc
	$(CC) $(CFLAGS) node/conn.cpp -o $@
$(BIN)nnode.o: ../inc/node.h ../lib/inc
	$(CC) $(CFLAGS) node/node.cpp -o $@

# crypt
$(BIN)lockmsg.o: ../inc/crypt++.h
	$(CC) $(CFLAGS) crypt++/lockmsg.cpp -o $@

$(BIN)DSA.o: ../inc/crypt++.h
	$(CC) $(CFLAGS) crypt++/DSA.cpp -o $@

$(BIN)RSA.o: ../inc/crypt++.h
	$(CC) $(CFLAGS) crypt++/RSA.cpp -o $@

$(BIN)AES.o: ../inc/crypt++.h
	$(CC) $(CFLAGS) crypt++/AES.cpp -o $@

clean:
	cd $(BIN); rm -rf *o main
	rm -f libconcord.so
