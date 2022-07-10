
default: rebuild

CC = g++ -std=c++20
CLFAG = -g -Wall

# protocol dir
P = ./../../proto
# lib dir
L = ./../../lib
# core dir
C = ./../../build/exe

# include dump
INC = -I$(P)/hclc/inc -I$(C)/inc -I$(L)/inc

# link dump
LNK = -L$(P)/hclc/bin/ -L$(L) -L$(C)

rebuild: clean build

build:	
	$(CC) $(CFLAG) $(INC) main.cpp -o ln.out $(LNK) -lhclc -lcore -lcryptopp -lpthread -luttu

clean:
	rm -rf achain/* bchain/* ln.out
