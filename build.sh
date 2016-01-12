#!/bin/sh

BINDIR="../bin"
CFLAGS="-std=c99 -Wall -pedantic -march=native -g"
LIBS="SDL2"
EXE="titan"

# build titan
echo "Building titan..."
gcc *.c $CFLAGS -l$LIBS -o $BINDIR/$EXE

# build tscript
echo "Building tscript..."
gcc tscript/*.c $CFLAGS -o $BINDIR/tscript
