#!/bin/bash

set -e

NUM=100000
SHM_BUFSIZE=$1
shift

REP=$1
export PYTHON_OPTIMIZE=1
#export PYTHONDONTWRITEBYTECODE=1

for ARBITER_BUFSIZE in 4 16 32 64 128 256; do
	echo "** SHM BUFSIZE: $SHM_BUFSIZE, ARBITER_BUFSIZE: $ARBITER_BUFSIZE **"
	echo " - Generating the monitor"
	make -j4 BUFSIZE=$ARBITER_BUFSIZE 1>>/tmp/make.stdout.txt 2>>/tmp/make.stderr.txt

	for i in `seq 1 $REP`; do
	echo " - Running"
	./regexbank.sh $NUM $ARBITER_BUFSIZE >out-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-$i.txt 2>err-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-$i.txt
	pyhthon3 parse_results.py out-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-$i.txt 0 >> bank-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM.csv
	done

	for ERR_FREQ in 5 10 100 1000; do
		for i in `seq 1 $REP`; do
	    echo " - Running with $ERR_FREQ errors freq"
	    ./regexbank.sh $NUM $ARBITER_BUFSIZE errors $ERR_FREQ >out-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-ERR-$ERR_FREQ-$i.txt 2>err-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-ERR-$ERR_FREQ-$i.txt
	    ./parse_results.py out-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-ERR-$ERR_FREQ-$i.txt $ERR_FREQ >> bank-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-ERR-$ERR_FREQ.csv
		done
    done
done

