# WIP, the below doesn't work. Don't try to make this.
 
CC = g++
CFLAGS = -Wall -g -lssl -lcrypt -lcrypto
  
main: main.o utils.o
	$(CC) -o main main.o utils.o $(CFLAGS)
  
main.o: main.cpp utils.h
	$(CC) -c main.cpp $(CFLAGS)

utils.o: utils.h
