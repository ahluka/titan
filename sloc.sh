CTOTAL=$(wc -l *.c *.h gus/*.c gus/*.h | grep total)
CFILECNT=$(find . -type f -iname "*.c" -o -iname "*.h" | wc -l)
SCRTOTAL=$(wc -l ../bin/res/tcl/*.tcl | grep total)
SCRFILECNT=$(find ../bin/res/tcl -maxdepth 1 -type f -iname "*.tcl" | wc -l)

echo "$CTOTAL C code in $CFILECNT files"
echo "$SCRTOTAL script code in $SCRFILECNT files"
