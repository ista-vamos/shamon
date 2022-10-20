#!/bin/bash

set -e

DIR=$(dirname $0)

SOURCESDIR="$DIR/../../sources"
NUM=$1
test -z $NUM && NUM=10000

EXE=$DIR/primes

CMAKE_CACHE="$DIR/../../CMakeCache.txt"
LINE=$(grep "DynamoRIO_DIR" "$CMAKE_CACHE")
DRIOROOT="${LINE#*=}/.."
# fallback for our machine...
if [ ! -d $DRIOROOT ]; then
	DRIOROOT=/opt/dynamorio/build
fi

DRRUN="$DRIOROOT/bin64/drrun"
if [ ! -x $DRRUN ]; then
	echo "Could not find drrun"
	exit 1
fi

$DRRUN -root $DRIOROOT \
	-opt_cleancall 2 -opt_speed\
	-c $SOURCESDIR/drregex/libdrregex.so\
	/primes1 prime '#([0-9]+): ([0-9]+)' ii --\
	$EXE $NUM &

$DRRUN -root $DRIOROOT \
	-opt_cleancall 2 -opt_speed\
	-c $SOURCESDIR/drregex/libdrregex.so\
	/primes2 prime '#([0-9]+): ([0-9]+)' ii --\
	$EXE-bad $NUM 10&

wait
wait
