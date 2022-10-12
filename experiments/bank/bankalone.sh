#!/bin/bash

set -e

NUM=$1
shift

rm -f /tmp/fifo{A,B}
mkfifo /tmp/fifo{A,B}

python3 inputs.py $NUM > inputs.last.txt

$(dirname $0)/bank $@ < /tmp/fifoA  > /tmp/fifoB 2>source-log.txt&
BANK_PID=$!

cat /tmp/fifoB | ./interact inputs.last.txt interact.log >/tmp/fifoA &

wait $BANK_PID

rm -f /tmp/fifo{A,B}

cat interact.log
