OBJS=*.c
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -march=native -g
#release flags
#CFLAGS=-std=c99 -Wall -O2 -Os
LIBS=SDL2
EXE=titan

all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -l$(LIBS) -o ../bin/$(EXE)
