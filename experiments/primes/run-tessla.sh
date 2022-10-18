#!/bin/bash

set -e  # exit on any error

DIR=$(dirname $0)

SHM_BUFSIZE=$1
ARBITER_BUFSIZE=$2
PRIMES_NUM=$3
TYPE=$4

VAMOS=$DIR/vamos-tessla
MONITOR=$DIR/tessla-monitor

rm -f /tmp/tessla.in /dev/shm/primes*
mkfifo /tmp/tessla.in


./runprimes-dr.sh $PRIMES_NUM >primes.stdout.log 2>primes.stderr.log&
PRIMES_PID=$!

# vamos feeds data to /tmp/tessla.in
$VAMOS P_0:regex:/primes1 P_1:regex:/primes2 &
VAMOS_PID=$!

$MONITOR < /tmp/tessla.in > tessla.out

CHECKED=$(cat tessla.out | wc -l)
ERRS=$(grep "ok = false" tessla.out | wc -l)

echo "$CHECKED,$ERRS" >> tessla-$TYPE-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$PRIMES_NUM.csv

wait $VAMOS_PID
wait $PRIMES_PID

rm -f /tmp/tessla.in
