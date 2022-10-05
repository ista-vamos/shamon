#!/bin/bash

set -e

cd $(dirname 0)

rm -f log.txt

SRCDIR="$(dirname $0)/../.."
SHM_BUFSIZE_FILE="${SRCDIR}/shmbuf/buffer-size.h"


REP=1

echo "Logs of make are in /tmp/make*.txt"

for i in `seq 1 $REP`; do
for SHM_BUFSIZE in 1 8 16 32; do
	echo "make clean"
	make clean 1>/tmp/make-clean.stdout.txt
        make clean -j -C $SRCDIR 1>>/tmp/make-clean.stdout.txt
        sed -i "s/#define\\s*SHM_BUFFER_SIZE_PAGES.*/#define SHM_BUFFER_SIZE_PAGES $SHM_BUFSIZE/" $SHM_BUFSIZE_FILE
	echo "make all"
	make -j -C "$SRCDIR"  1>/tmp/make.stdout.txt

	echo "make source"
	make source -B 1>>/tmp/make.stdout.txt

	echo "-- run.sh --"
        ./run.sh $SHM_BUFSIZE
done
done
