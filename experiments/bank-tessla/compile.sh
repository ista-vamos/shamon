#!/bin/bash

DIR=$(dirname $0)
SRCDIR="$DIR/../../"
VLCC="python3 $SRCDIR/compiler/main.py"
GENCC=$SRCDIR/gen/compile.sh
LIBMONITOR=$SRCDIR/tessla/bankmon/target/debug/libmonitor.a

if [ ! -f bankmonitor.vl ]; then
	sed 's/@BUFSIZE/1024/' bankmonitor.vl.in > bankmonitor.vl
fi

$VLCC bankmonitor.vl monitor.c
g++ -O3 $SRCDIR/compiler/cfiles/intmap.cpp -c
#$GENCC $DIR/bankmontessla.c $DIR/intmap.o -lstdc++ $LIBMONITOR -ldl
$GENCC $DIR/monitor.c $DIR/intmap.o -lstdc++ $LIBMONITOR -ldl
