SHELL = /bin/sh

# local path for the lib folder *Change Based on Env*
D = ./../lib

# cpp standard
CC = g++ -std=c++20

# compile-time flags
CFLAGS = -c -g -Wall -fPIC

# lib flags
L = -lcryptopp -pthread

# object file sub-dir of src
BIN = _bin/

# .so args
SOFLAGS = -shared -Wl,-soname,libconcord.so

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
  $(BIN)nhclc.o \
	$(BIN)nconn.o \
	$(BIN)nnode.o

default: static_unix

install_unix: shared_unix
	@echo "INSTALLING FOR UNIX"
	sudo cp libconcord.so /usr/lib/libconcord.so
	sudo ldconfig
	sudo mkdir -p /usr/include/concord
	sudo cp ../inc/* /usr/include/concord

install_clean: clean
	sudo rm -f /usr/lib/libconcord.so
	sudo ldconfig
	sudo rm -rf /usr/include/concord

shared_unix: $(OBJ)
	@echo "NOW BUILDING (UNIX) | SHARED OBJECT"
	$(CC) $(SOFLAGS) $(OBJ) $(D)/libuttu.o $(L) -o libconcord.so

static_unix: $(OBJ)
	@echo "NOW LINKING (UNIX) | ARCHIVE"
	ar cr libcore.a $(OBJ) $(D)/libuttu.o
	ranlib libcore.a
	mv libcore.a ../exe/
	cp -r ../inc ../exe/
	cp $(D)/inc/* ../exe/inc

# string manipulation
$(BIN)b64.o: ../inc/strops.hpp
	$(CC) $(CFLAGS) strops/b64.cpp -o $@
$(BIN)hexstr.o: ../inc/strops.hpp
	$(CC) $(CFLAGS) strops/hexstr.cpp -o $@
$(BIN)strutil.o: ../inc/strops.hpp
	$(CC) $(CFLAGS) strops/strutil.cpp -o $@
$(BIN)hash.o: ../inc/strops.hpp
	$(CC) $(CFLAGS) strops/hash.cpp -o $@
$(BIN)miner.o: ../inc/strops.hpp
	$(CC) $(CFLAGS) strops/miner.cpp -o $@
$(BIN)tree.o: ../inc/tree.hpp
	$(CC) $(CFLAGS) tree/tree.cpp -o $@

# chain utils
$(BIN)ttime.o: ../inc/tree.hpp
	$(CC) $(CFLAGS) tree/time_enc.cpp -o $@
$(BIN)tbenc.o: ../inc/tree.hpp
	$(CC) $(CFLAGS) tree/block_enc.cpp -o $@

# server abstraction
$(BIN)server.o: ../inc/server.hpp
	$(CC) $(CFLAGS) server/server.cpp -o $@
$(BIN)smisc.o: ../inc/server.hpp
	$(CC) $(CFLAGS) server/claf_misc.cpp -o $@
$(BIN)sbranch_ctx.o: ../inc/server.hpp
	$(CC) $(CFLAGS) server/branch_ctx.cpp -o $@

# nodes
$(BIN)nhclc.o: ../inc/node.hpp
	$(CC) $(CFLAGS) -I$(D)/inc node/hclc.cpp -o $@
$(BIN)nconn.o: ../inc/node.hpp
	$(CC) $(CFLAGS) -I$(D)/inc node/conn.cpp -o $@
$(BIN)nnode.o: ../inc/node.hpp
	$(CC) $(CFLAGS) -I$(D)/inc node/node.cpp -o $@

# crypt
$(BIN)lockmsg.o: ../inc/crypt++.hpp
	$(CC) $(CFLAGS) crypt++/lockmsg.cpp -o $@
$(BIN)DSA.o: ../inc/crypt++.hpp
	$(CC) $(CFLAGS) crypt++/DSA.cpp -o $@
$(BIN)RSA.o: ../inc/crypt++.hpp
	$(CC) $(CFLAGS) crypt++/RSA.cpp -o $@
$(BIN)AES.o: ../inc/crypt++.hpp
	$(CC) $(CFLAGS) crypt++/AES.cpp -o $@

# have to force b/c unknown lib type
clean:
	rm $(BIN)*.o
	rm ../exe/inc/*.hpp
	rm -f ../exe/*.a
	rm -f ../exe/*.so
