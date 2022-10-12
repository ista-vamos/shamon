#!/bin/bash

set -e

NUM=100000
SHM_BUFSIZE=$1
shift

REP=1
export PYTHON_OPTIMIZE=1
#export PYTHONDONTWRITEBYTECODE=1

for i in `seq 1 $REP`; do
    for ARBITER_BUFSIZE in 4 16 32 64 128 256; do
	echo "** SHM BUFSIZE: $SHM_BUFSIZE, ARBITER_BUFSIZE: $ARBITER_BUFSIZE **"
	echo " - Generating the monitor"
	./genmon.py bankmonitor.vl.in $ARBITER_BUFSIZE
	make -j4 bank 1>>/tmp/make.stdout.txt 2>>/tmp/make.stderr.txt
	make -j4 interact 1>>/tmp/make.stdout.txt 2>>/tmp/make.stderr.txt
	make -j4 monitor-vamos 1>>/tmp/make.stdout.txt 2>>/tmp/make.stderr.txt

	echo " - Running"
	rm -f out.txt err.txt
	./regexbank.sh $NUM >out.txt 2>err.txt
	./parse_results.py out.txt 0 >> bank-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM.csv

	for ERR_FREQ in 5 10 100 1000; do
	    echo " - Running with $ERR_FREQ errors freq"
	    ./regexbank.sh $NUM errors $ERR_FREQ >out.txt 2>err.txt
	    ./parse_results.py out.txt $ERR_FREQ >> bank-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM.csv
	done
    done
done

