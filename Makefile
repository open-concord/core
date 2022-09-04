SHELL = /bin/sh

# local path for the lib folder *Change Based on Env*
D = ./lib/uttu

# cpp standard
CC = g++

# compile-time flags
CFLAGS = -std=c++20 -c -g -fPIC

# warn level
W = -Wall

## gcc chunk ##
G = $(CC) $(CFLAGS) $(W)

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
	$(BIN)tload.o \
	$(BIN)tutil.o \
	$(BIN)block.o \
	$(BIN)b64.o \
	$(BIN)hexstr.o \
	$(BIN)ctx.o	 \
	$(BIN)gboiler.o \
	$(BIN)gpush.o

default: static

install: shared
	@echo "-- INSTALLING --"
	sudo cp libconcord.so /usr/lib/libconcord.so
	sudo ldconfig
	sudo mkdir -p /usr/include/concord
	sudo cp ./inc/* /usr/include/concord

shared: $(OBJ) 
	@echo "-- NOW BUILDING | SHARED OBJECT --"
	$(CC) $(SOFLAGS) $(OBJ) $(D)/libuttu.o $(L) -o libconcord.so

static: $(OBJ) 
	@echo "EXTRACTING FROM UTTU"
	ar xv $(D)/libuttu.a --output $(BIN)
	@echo "-- NOW BUILDING | ARCHIVE --"
	ar cr libcore.a $(wildcard $(BIN)*.o)
	ranlib libcore.a	 
	mv libcore.a ./build/exe/
	cp -r ./inc ./build/exe/
	cp $(D)/inc/* ./build/exe/inc
	@echo "LIBCORE CREATION COMPLETE"

# graph model
$(BIN)gboiler.o: ./inc/graph.hpp
	$(G) ./src/graph/boiler.cpp -o $@
$(BIN)gpush.o: ./inc/graph.hpp
	$(G) ./src/graph/push.cpp -o $@

# string manipulation
$(BIN)b64.o: ./inc/strops.hpp
	$(G) ./src/strops/b64.cpp -o $@
$(BIN)hexstr.o: ./inc/strops.hpp
	$(G) ./src/strops/hexstr.cpp -o $@
$(BIN)hash.o: ./inc/strops.hpp
	$(G) ./src/strops/hash.cpp -o $@
$(BIN)miner.o: ./inc/strops.hpp
	$(G) ./src/strops/miner.cpp -o $@

# graph utils
$(BIN)tree.o: ./inc/tree.hpp
	$(G) ./src/tree/tree.cpp -o $@
$(BIN)tload.o: ./inc/tree.hpp
	$(G) ./src/tree/load.cpp -o $@
$(BIN)tutil.o: ./inc/tree.hpp
	$(G) ./src/tree/util.cpp -o $@
$(BIN)time.o: ./inc/tree.hpp
	$(G) ./src/tree/time_enc.cpp -o $@
$(BIN)block.o: ./inc/tree.hpp
	$(G) ./src/tree/block.cpp -o $@

# nodes
$(BIN)ctx.o: ./inc/ctx.hpp
	$(G) -I$(D)/inc ./src/ctx/node.cpp -o $@

# crypt
$(BIN)lockmsg.o: ./inc/crypt.hpp
	$(G) ./src/crypt/lockmsg.cpp -o $@
$(BIN)DSA.o: ./inc/crypt.hpp
	$(G) ./src/crypt/DSA.cpp -o $@
$(BIN)RSA.o: ./inc/crypt.hpp
	$(G) ./src/crypt/RSA.cpp -o $@
$(BIN)AES.o: ./inc/crypt.hpp
	$(G) ./src/crypt/AES.cpp -o $@

# have to force b/c unknown lib type
clean:
	rm -f $(BIN)*.o
	rm -f ./build/exe/inc/*.hpp
	rm -f ./build/exe/*.a
	rm -f ./build/exe/*.so

reset: clean
	rm -rf ./lib/*
	touch ./lib/"PLACEHOLDER"

# v huh
FORCE: ;
