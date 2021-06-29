#
# This is an example Makefile for a countwords program.  This
# program uses both the scanner module and a counter module.
# Typing 'make' or 'make count' will create the executable file.
#

# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = gcc
CFLAGS  = -g -Wall
LFLAGS  = -lssl -lcrypt -lcrypto

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: main

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#
main:  main.cpp tree.o
	$(CC) $(CFLAGS) -o main tree.o main.cpp

tree.o:  tree.cpp tree.h miner.o hash.o
	$(CC) $(CFLAGS) -c hash.o miner.o tree.cpp $(LFLAGS)

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