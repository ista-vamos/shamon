#!/bin/bash

set -e

REP=3

cd $(dirname $0)
rm -f results-*.csv

for I in `seq 1 $REP`; do
	for FILE in $(ls benchmarks/*.i); do
		python3 ./run.py $FILE results-$I.csv;
	done
done
