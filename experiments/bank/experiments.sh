#!/bin/bash

set -e

cd $(dirname 0)

rm -f log.txt

SRCDIR="$(dirname $0)/../.."

source ../setup.sh

echo "Logs of make are in /tmp/make*.txt"

for i in `seq 1 $REPEAT`; do
	echo "make clean"
	make clean 1>/tmp/make-clean.stdout.txt 2>/tmp/make-clean.stderr.txt
        make clean -j -C $SRCDIR 1>>/tmp/make-clean.stdout.txt 2>>/tmp/make-clean.stderr.txt
	echo "make shamon"
	make -j -C "$SRCDIR"  1>/tmp/make.stdout.txt 2>/tmp/make.stderr.txt

	echo "-- run.sh --"
        ./run.sh
done
