SHELL = /bin/sh

# c/cpp standard
CC = g++ -std=c++20
# compile-time flags
C = -g -Wall

# project dir
D = ./../exe

tnode:
	$(CC) $(C) -I$(D)/inc src/tnode.cpp -L$(D) -lcore -lcryptopp -lpthread -o output/$@.out

tcrypt++:
	$(CC) $(CFLAGS) src/$@.cpp -o output/$@ -lcore

test:
	$(CC) $(CFLAGS) src/$@.cpp -o output/$@ -lcore
