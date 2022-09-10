default: static

# local path for the lib folder *Change Based on Env*
D = ./lib/uttu/

# cpp standard
G = g++

# warn level
W = -Wall

# compile-time flags
CF = -std=c++20 $(W) -c -fPIC -MMD -MP

## gcc chunk ##
CC = $(G) $(CF) -I$(D)inc

# lib flags
L = -lcryptopp -pthread 

# .so args
SOFLAGS = -shared -Wl,-soname,libconcord.so

# objects
SRC = $(wildcard src/**/*.cpp)
HDR = $(wildcard inc/*.hpp)
B = ./build/bin/

OBUILD:
	@echo "-- BUILDING SRC --"
	$(foreach f,$(SRC),\
		$(CC) $(CF) $f -o $(B)$(lastword $(subst /, , $(basename $f))).o $(L); \
		echo "Built - $f"; \
	)

install: shared
	@echo "-- INSTALLING --"
	sudo cp libconcord.so /usr/lib/libconcord.so
	sudo ldconfig
	sudo mkdir -p /usr/include/concord
	sudo cp ./inc/* /usr/include/concord

shared: OBUILD 
	@echo "-- NOW BUILDING | SHARED OBJECT --"
	$(CC) $(SOFLAGS) $(OBJ) $(D)/libuttu.o $(L) -o libconcord.so

static: OBUILD 
	@echo "-- EXTRACTING | UTTU --"
	ar xv $(D)/libuttu.a --output $(B)
	@echo "-- NOW BUILDING | ARCHIVE --"
	ar cr libcore.a $(wildcard $(B)*.o)
	ranlib libcore.a	 
	mv libcore.a ./build/exe/
	cp -r ./inc ./build/exe/
	cp $(D)/inc/* ./build/exe/inc
	@echo "LIBCORE CREATION COMPLETE"

# have to force b/c unknown lib type
clean:
	rm -f $(B)*.o
	rm -f ./build/exe/inc/*.hpp
	rm -f ./build/exe/*.a
	rm -f ./build/exe/*.so

reset: clean
	rm -rf ./lib/*
	touch ./lib/"PLACEHOLDER"

# v huh
FORCE: ;
