CC = g++
CFLAGS  = -g -Wall
LFLAGS  = -lssl -lcrypt -lcrypto

default: main

main:  main.cpp tree.o
	$(CC) $(CFLAGS) -o main tree.o main.cpp

tree.o:  tree.cpp tree.h miner.o hash.o timewizard.o
	$(CC) $(CFLAGS) -c hash.o timewizard.o miner.o tree.cpp $(LFLAGS)

miner.o:  miner.cpp miner.h hash.o
	$(CC) $(CFLAGS) -c hash.o miner.cpp $(LFLAGS)

hash.o: hash.cpp hash.h
	$(CC) $(CFLAGS) -c hash.cpp $(LFLAGS)

timewizard.o:  timewizard.cpp timewizard.h 
	$(CC) $(CFLAGS) -o timewizard.cpp $(LFLAGS)

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) count *.o *~