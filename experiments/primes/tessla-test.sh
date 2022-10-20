#!/bin/bash

set -e  # exit on any error

DIR=$(dirname $0)

SRCDIR="$(readlink -f $(dirname $0)/../..)"
SHM_BUFSIZE_FILE="${SRCDIR}/shmbuf/buffer-size.h"
VLCC="python3 $SRCDIR/compiler/main.py"
GENCC="$SRCDIR/gen/compile.sh"
TESSLA_JAR=/opt/vamos/tessla/tessla-rust.jar
# compile the tessla monitor
#java -jar $TESSLA_JAR compile-rust primes.tessla -b tessla-monitor


SPEC="primes-tessla.txt"
SPEC_IN="primes-align-tessla.txt.in"
# generate monitor spec
sed s@ARBITER_BUFSIZE@2048@ $SPEC_IN > $SPEC
sed -i s@SRCDIR@$SRCDIR@ $SPEC
# compile it
$VLCC $SPEC monitor-tessla.c
$GENCC monitor-tessla.c ./intmap.o $SRCDIR/compiler/cfiles/compiler_utils.c -lstdc++
mv monitor vamos-tessla

VAMOS=$DIR/vamos-tessla
MONITOR=$DIR/tessla-monitor

rm -f /tmp/tessla.in 
mkfifo /tmp/tessla.in


# vamos feeds data to /tmp/tessla.in
$VAMOS P_0:regex:/primes1 P_1:regex:/primes2 &
VAMOS_PID=$!

./runprimes-dr-bad.sh $1 >primes.stdout.log 2>primes.stderr.log&
PRIMES_PID=$!

/bin/time -f '%e' -o tessla.time $MONITOR < /tmp/tessla.in > tessla.out

wait $VAMOS_PID
wait $PRIMES_PID

CHECKED=$(cat tessla.out | wc -l)
ERRS=$(grep "ok = false" tessla.out | wc -l)

echo "Checked $CHECKED"
echo "Found errors" $ERRS
grep 'Errors generated' primes.stderr.log
echo "Monitor time $(cat tessla.time)"

rm -f /tmp/tessla.in
