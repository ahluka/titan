#!/bin/sh

# I hate make.
# I hate it so much I'd rather just throw every source file at gcc every time
# and wait slightly longer for it all to build.
# So that's what I do.

BINDIR="../bin"
# _POSIX_C_SOURCE is defined to enable strdup() which isn't part of C99
# TODO: Remove _POSIX_C_SOURCE? Replaced strdup(), for now...
CFLAGS="-std=c99 -Wall -pedantic -march=native -g -D_POSIX_C_SOURCE=200809L"
LIBS="-lSDL2 -lSDL2_image -lSDL2_ttf -llua5.3 -L./gus/ -lgus"
EXE="titan"

# build gus as a static library
gcc gus/*.c $CFLAGS -c -o gus/gus.o
ar rcs gus/libgus.a gus/gus.o

# build titan
echo "Building $EXE..."
gcc *.c $CFLAGS $LIBS -o $BINDIR/$EXE
