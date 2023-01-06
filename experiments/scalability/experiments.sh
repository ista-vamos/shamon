#!/bin/bash

set -e

cd $(dirname 0)

source ../setup.sh

rm -f log.txt

export PYTHON_OPTIMIZE=1

SRCDIR="$(dirname $0)/../.."

echo "Logs of make are in /tmp/make*.txt"

for i in `seq 1 $REPEAT`; do
# the numbers correspond to 1 8 16 32 pages of memory
#for SHM_BUFSIZE in 145 1340 2705 5436; do
# run only for 8 pages (= 1340 elements) if not said otherwise
for SHM_BUFSIZE in 1340 ; do
	echo "make clean"
	make clean 1>/tmp/make-clean.stdout.txt
        make clean -j -C $SRCDIR 1>>/tmp/make-clean.stdout.txt
	echo "make all"
	make -j -C "$SRCDIR"  1>/tmp/make.stdout.txt

	echo "make source"
	make source -B 1>>/tmp/make.stdout.txt

	echo "-- run --"
        ./run.py $SHM_BUFSIZE $SCALABILITY_NUM
done
done
