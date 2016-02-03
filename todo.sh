echo "==== TODO ===="
grep --color -n TODO *.c *.h gus/*.c gus/*.h

echo ""
echo "==== FIXME ===="
grep --color -n FIXME *.c *.h
