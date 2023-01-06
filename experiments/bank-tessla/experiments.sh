#!/bin/bash

set -e

cd $(dirname 0)

rm -f log.txt

SRCDIR="$(dirname $0)/../.."

source ../setup.sh

# RUNDIR="$(dirname $0)/rundata$(date +%Y%m%d-%H%M%S)"
# mkdir $RUNDIR
RUNDIR="$(dirname $0)"

echo "Logs of make are in /tmp/make*.txt"

echo "make clean"
make clean 1>/tmp/make-clean.stdout.txt 2>/tmp/make-clean.stderr.txt
make clean -j -C $SRCDIR 1>>/tmp/make-clean.stdout.txt 2>>/tmp/make-clean.stderr.txt
echo "make shamon"
make -j -C "$SRCDIR"  1>/tmp/make.stdout.txt 2>/tmp/make.stderr.txt

./run.sh $REPEAT "$RUNDIR"
