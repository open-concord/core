SHELL = /bin/sh

# local path for the lib folder *Change Based on Env*
D = ./lib

# cpp standard
CC = g++ -std=c++20

# compile-time flags
CFLAGS = -c -g -Wall -fPIC

# lib flags
L = -lcryptopp -pthread 

# object file dump
BIN = ./build/bin/

# .so args
SOFLAGS = -shared -Wl,-soname,libconcord.so

# objects
OBJ = $(BIN)lockmsg.o \
	$(BIN)AES.o \
	$(BIN)RSA.o \
	$(BIN)DSA.o \
	$(BIN)hash.o \
	$(BIN)miner.o \
	$(BIN)time.o \
	$(BIN)tree.o \
	$(BIN)block.o \
	$(BIN)b64.o \
	$(BIN)hexstr.o \
	$(BIN)ctx.o	

default: static_unix

install_unix: shared_unix
	@echo "INSTALLING FOR UNIX"
	sudo cp libconcord.so /usr/lib/libconcord.so
	sudo ldconfig
	sudo mkdir -p /usr/include/concord
	sudo cp ./inc/* /usr/include/concord

install_clean: clean
	sudo rm -f /usr/lib/libconcord.so
	sudo ldconfig
	sudo rm -rf /usr/include/concord

shared_unix: $(OBJ) protocols
	@echo "NOW BUILDING (UNIX) | SHARED OBJECT"
	$(CC) $(SOFLAGS) $(OBJ) $(D)/libuttu.o $(L) -o libconcord.so

static_unix: $(OBJ) proto
	@echo "EXTRACTING FROM UTTU"
	ar xv $(D)/libuttu.a --output $(BIN)
	@echo "NOW COMPILING (UNIX) | ARCHIVE"
	ar cr libcore.a $(wildcard $(BIN)*.o)
	ranlib libcore.a	 
	mv libcore.a ./build/exe/
	cp -r ./inc ./build/exe/
	cp $(D)/inc/* ./build/exe/inc
	@echo "LIBCORE CREATION COMPLETE"

# string manipulation
$(BIN)b64.o: ./inc/strops.hpp
	$(CC) $(CFLAGS) ./src/strops/b64.cpp -o $@
$(BIN)hexstr.o: ./inc/strops.hpp
	$(CC) $(CFLAGS) ./src/strops/hexstr.cpp -o $@
$(BIN)hash.o: ./inc/strops.hpp
	$(CC) $(CFLAGS) ./src/strops/hash.cpp -o $@
$(BIN)miner.o: ./inc/strops.hpp
	$(CC) $(CFLAGS) ./src/strops/miner.cpp -o $@

# chain utils
$(BIN)tree.o: ./inc/tree.hpp
	$(CC) $(CFLAGS) ./src/tree/tree.cpp -o $@
$(BIN)time.o: ./inc/tree.hpp
	$(CC) $(CFLAGS) ./src/tree/time_enc.cpp -o $@
$(BIN)block.o: ./inc/tree.hpp
	$(CC) $(CFLAGS) ./src/tree/block.cpp -o $@

# nodes
$(BIN)ctx.o: ./inc/ctx.hpp
	$(CC) $(CFLAGS) -I$(D)/inc ./src/ctx/node.cpp -o $@

# crypt
$(BIN)lockmsg.o: ./inc/crypt.hpp
	$(CC) $(CFLAGS) ./src/crypt/lockmsg.cpp -o $@
$(BIN)DSA.o: ./inc/crypt.hpp
	$(CC) $(CFLAGS) ./src/crypt/DSA.cpp -o $@
$(BIN)RSA.o: ./inc/crypt.hpp
	$(CC) $(CFLAGS) ./src/crypt/RSA.cpp -o $@
$(BIN)AES.o: ./inc/crypt.hpp
	$(CC) $(CFLAGS) ./src/crypt/AES.cpp -o $@

# build protocols
PDIR = proto
proto: FORCE
	for p in $(PDIR)/*; do \
		cd $$p; $(MAKE); cd ../..;	\
	done
# clean protocols
pclean:
	for p in $(PDIR)/*; do \
		cd $$p; $(MAKE) clean; cd ../..; \
		done

# have to force b/c unknown lib type
clean:
	rm -f $(BIN)*.o
	rm -f ./build/exe/inc/*.hpp
	rm -f ./build/exe/*.a
	rm -f ./build/exe/*.so
	for p in $(PDIR)/*; do \
		cd $$p; rm -f bin/*.o; cd ../..; \
	done

reset: clean
	rm -rf ./lib/*
	touch ./lib/"PLACEHOLDER"

FORCE: ;
