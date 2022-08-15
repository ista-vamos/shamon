#!/bin/bash

set -e

DIR="$(dirname $0)"
SOURCESDIR="$DIR/../../sources"
REGEXSOURCE=$SOURCESDIR/regex
NUM=$1
test -z $NUM && NUM=10000

# check that `primes` exists and is executable
if [ ! -x $DIR/primes ]; then
	echo "'primes' executable does not exist or has no x permission"
	echo "maybe run 'make' in '${DIR}'?"
	exit 1
fi

$DIR/primes $NUM | $REGEXSOURCE /primes1 prime "#([0-9]+):\s*([0-9]+)\s*$" ii&
$DIR/primes $NUM | $REGEXSOURCE /primes2 prime "#([0-9]+):\s*([0-9]+)\s*$" ii&

wait
wait
