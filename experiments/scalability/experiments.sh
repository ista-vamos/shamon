#!/bin/bash

set -e

cd $(dirname 0)

rm -f log.txt

SRCDIR="$(dirname $0)/../.."


REP=1

echo "Logs of make are in /tmp/make*.txt"

for i in `seq 1 $REP`; do
# the numbers correspond to 1 8 16 32 pages of memory
#for SHM_BUFSIZE in 145 1340 2705 5436; do
# run only for 1 page if not said otherwise
for SHM_BUFSIZE in 1340 ; do
	echo "make clean"
	make clean 1>/tmp/make-clean.stdout.txt
        make clean -j -C $SRCDIR 1>>/tmp/make-clean.stdout.txt
	echo "make all"
	make -j -C "$SRCDIR"  1>/tmp/make.stdout.txt

	echo "make source"
	make source -B 1>>/tmp/make.stdout.txt

	echo "-- run.sh --"
        ./run.sh $SHM_BUFSIZE
done
done
