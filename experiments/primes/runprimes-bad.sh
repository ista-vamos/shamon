#!/bin/bash

SOURCESDIR="$(pwd)/../../sources"
REGEXSOURCE=$SOURCESDIR/regex
NUM=$1
test -z $NUM && NUM=10000
./primes     $NUM              | $REGEXSOURCE /primes1 prime "#([0-9]+):\s*([0-9]+)\s*$" ii&
./primes-bad $NUM $(($NUM/10)) | $REGEXSOURCE /primes2 prime "#([0-9]+):\s*([0-9]+)\s*$" ii&

wait
wait
