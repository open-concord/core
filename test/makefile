SHELL = /bin/sh
# fix any issues w/ suffixes
.SUFFIXES: .cpp .o

# c/cpp standard
CC = g++
# compile-time flags
CFLAGS = -g -Wall

# calls for make; ex. `~/Concord/main/test$ make tnode`
tnode:
	$(CC) $(CFLAGS) src/tnode.cpp -o output/$@ -lconcord

tcrypt++:
	$(CC) $(CFLAGS) src/$@.cpp -o output/$@ -lconcord

test:
	$(CC) $(CFLAGS) src/$@.cpp -o output/$@ -lconcord