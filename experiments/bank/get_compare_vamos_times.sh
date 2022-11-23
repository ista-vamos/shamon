#!/bin/bash

REP=10

echo "type,time,monitor" > bank_times.csv
for i in `seq 1 $REP`; do

    bash regexbank.sh 10000 10
done

mv bank_times.csv ../tessla_compare/bank_times2.csv
rm tmp.file
