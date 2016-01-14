#!/bin/sh

BINDIR="../bin"
# _POSIX_C_SOURCE is defined to enable strdup() which isn't part of C99
CFLAGS="-std=c99 -Wall -pedantic -march=native -g -D_POSIX_C_SOURCE=200809L"
LIBS="-lSDL2 -lSDL2_image -lSDL2_ttf"
EXE="titan"
SCRIPTEXE="tscript"

# build titan
echo "Building $EXE..."
gcc *.c $CFLAGS $LIBS -o $BINDIR/$EXE

# build tscript
#echo "Building $SCRIPTEXE..."
#gcc tscript/*.c $CFLAGS -o $BINDIR/$SCRIPTEXE
