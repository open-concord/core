SHELL = /bin/sh

# c/cpp standard
CC = g++ -std=c++20
# compile-time flags
C = -g -Wall

# project dir
D = ./../exe

tnode:
	$(CC) $(C) -I$(D)/inc src/tnode.cpp -L$(D) -lconcord -lcryptopp -lpthread -o output/$@.out

tcrypt++:
	$(CC) $(CFLAGS) src/$@.cpp -o output/$@ -lconcord

test:
	$(CC) $(CFLAGS) src/$@.cpp -o output/$@ -lconcord

watson:
	$(CC) $(CFLAGS) -I$(D)/inc src/$@.cpp -o output/$(@) -L$(D) -lconcord -lcryptopp -lpthread
