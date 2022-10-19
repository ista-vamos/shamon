#!/bin/bash

DIR=$(dirname $0)
SRCDIR="$DIR/../../"
GENCC=$SRCDIR/gen/compile.sh
LIBMONITOR=$SRCDIR/tessla/bankmon/target/debug/libmonitor.a

g++ -O3 $SRCDIR/compiler/cfiles/intmap.cpp -c
$GENCC $DIR/bankmontessla.c $DIR/intmap.o -lstdc++ $LIBMONITOR -ldl
