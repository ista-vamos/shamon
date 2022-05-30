#!/bin/bash

SOURCESDIR="$(pwd)/../../sources"
NUM=$1
test -z $NUM && NUM=10000

PERF='/usr/lib/linux-tools/5.13.0-1011-intel/perf'
$PERF record \
/opt/dynamorio//build/bin64/drrun\
	-root /opt/dynamorio//build/\
	-opt_cleancall 2 -opt_speed\
	-c /opt/shamon/sources/drregex/libdrregex.so\
	/primes1 prime '#([0-9]+): ([0-9]+)' ii --\
	./primes $NUM &

/opt/dynamorio//build/bin64/drrun\
	-root /opt/dynamorio//build/\
	-opt_cleancall 2 -opt_speed\
	-c /opt/shamon/sources/drregex/libdrregex.so\
	/primes2 prime '#([0-9]+): ([0-9]+)' ii --\
	./primes $NUM&

wait
wait
