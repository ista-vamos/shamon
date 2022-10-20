#!/bin/bash

set -e  # exit on any error

DIR=$(dirname $0)

SHM_BUFSIZE=$1
ARBITER_BUFSIZE=$2
PRIMES_NUM=$3
TYPE=$4
BAD=$5

VAMOS=$DIR/vamos-tessla
MONITOR=$DIR/tessla-monitor

rm -f /tmp/tessla.in /dev/shm/primes*
mkfifo /tmp/tessla.in


if [ "$BAD" = "bad" ]; then
	./runprimes-dr-bad.sh $PRIMES_NUM >primes.stdout.log 2>primes.stderr.log&
else
	./runprimes-dr.sh $PRIMES_NUM >primes.stdout.log 2>primes.stderr.log&
fi
PRIMES_PID=$!

# vamos feeds data to /tmp/tessla.in
$VAMOS P_0:regex:/primes1 P_1:regex:/primes2 &
VAMOS_PID=$!

/bin/time -f '%e' -o tessla.time $MONITOR < /tmp/tessla.in > tessla.out

wait $VAMOS_PID
wait $PRIMES_PID

CHECKED=$(cat tessla.out | wc -l)
ERRS=$(grep "ok = false" tessla.out | wc -l)
ERRS_GENERATED=$(grep 'Errors generated' primes.stderr.log | cut -d ' ' -f 3)
TIME=$(cat tessla.time)

test -z $ERRS_GENERATED && ERRS_GENERATED=0

echo "$CHECKED,$ERRS,$ERRS_GENERATED,$TIME" >> tessla-$TYPE-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$PRIMES_NUM.csv
echo "$TYPE,$SHM_BUFSIZE,$ARBITER_BUFSIZE,$PRIMES_NUM,$CHECKED,$ERRS,$ERRS_GENERATED,$TIME" >> tessla-all.csv

rm -f /tmp/tessla.in
