SHELL = /bin/sh

# c/cpp standard
CC = g++ -std=c++20
# compile-time flags
C = -g -Wall

# project dir
D = ./../build/exe

# protocol dir
P = ./../proto

# lib dir
L = ./../lib

tnode:
	$(CC) $(C) -I$(D)/inc src/tnode.cpp -L$(D) -lconcord -lcryptopp -lpthread -o output/$@.out

tcrypt++:
	$(CC) $(CFLAGS) src/$@.cpp -o output/$@ -lconcord

test:
	$(CC) $(CFLAGS) src/$@.cpp -o output/$@ -lconcord

bell:
	$(CC) $(CFLAG) -I$(D)/inc -I$(P)/hclc/inc -I$(P)/claf/inc  -L$(P)/hclc/bin/ -L$(P)/claf/bin src/$(@).cpp -o output/$(@).out -L$(D) -L$(L) -lclaf -lhclc -lcore -lcryptopp -lpthread -luttu
