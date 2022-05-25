#!/bin/bash

SOURCESDIR="$(pwd)/../../sources"
REGEXSOURCE=$SOURCESDIR/regex
NUM=$1
test -z $NUM && NUM=10000

/home/marek/monitoring/dynamorio//build/bin64/drrun\
	-root /home/marek/monitoring/dynamorio//build/\
	-opt_cleancall 2 -opt_speed\
	-c /home/marek/monitoring/shamon/sources/drregex/libdrregex.so\
	/primes1 prime '#([0-9]+): ([0-9]+)' ii --\
	./primes $NUM &

/home/marek/monitoring/dynamorio//build/bin64/drrun\
	-root /home/marek/monitoring/dynamorio//build/\
	-opt_cleancall 2 -opt_speed\
	-c /home/marek/monitoring/shamon/sources/drregex/libdrregex.so\
	/primes2 prime '#([0-9]+): ([0-9]+)' ii --\
	python3 ./primes.py $NUM&

wait
wait
