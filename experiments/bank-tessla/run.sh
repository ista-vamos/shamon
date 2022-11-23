#!/bin/bash

set -e

NUM=10000
SHM_BUFSIZE=$1
shift

REP=$1
shift

RUNDIR=$1

export PYTHON_OPTIMIZE=1
#export PYTHONDONTWRITEBYTECODE=1

for ARBITER_BUFSIZE in 4 16 32 64 128 256 512 1024 2048; do
	echo "** SHM BUFSIZE: $SHM_BUFSIZE, ARBITER_BUFSIZE: $ARBITER_BUFSIZE **"
	echo " - Generating the monitor"
	make -j4 BUFSIZE=$ARBITER_BUFSIZE 1>> $RUNDIR/make.stdout.txt 2>> $RUNDIR/make.stderr.txt

	for i in `seq 1 $REP`; do
	echo " - Running"
	./regexbank.sh $NUM $ARBITER_BUFSIZE >$RUNDIR/out-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-$i.txt 2>$RUNDIR/err-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-$i.txt
	python3 parse_results.py $RUNDIR/out-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-$i.txt 0 >> $RUNDIR/bank-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM.csv
	done

	for ERR_FREQ in 5 10 100 1000; do
		for i in `seq 1 $REP`; do
	    echo " - Running with $ERR_FREQ errors freq"
	    ./regexbank.sh $NUM $ARBITER_BUFSIZE errors $ERR_FREQ >$RUNDIR/out-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-ERR-$ERR_FREQ-$i.txt 2>$RUNDIR/err-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-ERR-$ERR_FREQ-$i.txt
	    python3 parse_results.py $RUNDIR/out-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-ERR-$ERR_FREQ-$i.txt $ERR_FREQ >> $RUNDIR/bank-$SHM_BUFSIZE-$ARBITER_BUFSIZE-$NUM-ERR-$ERR_FREQ.csv
		done
    done
done

