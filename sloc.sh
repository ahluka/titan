CTOTAL=$(wc -l *.c *.h gus/*.c gus/*.h | grep total)
SCRTOTAL=$(wc -l ../bin/res/tcl/*.tcl | grep total)

echo "$CTOTAL C code"
echo "$SCRTOTAL script code"
