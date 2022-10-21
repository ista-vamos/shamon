#!/bin/bash

set -e

NUM=$1
shift
ARBITER_BUFSIZE=$1
shift

DIR=$(dirname $0)
BANK_DIR="$DIR/../bank"

DRIOPATH="/home/fabian/dynamorio/"
DRRUN="$DRIOPATH/build/bin64/drrun\
	-root $DRIOPATH/build/\
	-c ../../sources/drregex/libdrregex-mt.so"

MONITOR=$DIR/monitor$ARBITER_BUFSIZE
if [ $(basename "$0") == "regexbank-dump.sh" ]; then
	MONITOR=$DIR/../../monitors/monitor-generic
fi

rm -f /tmp/fifo{A,B}
rm -f /dev/shm/bank.{stdin,stdout}

mkfifo /tmp/fifo{A,B}

python3 $BANK_DIR/inputs.py $NUM > inputs.last.txt

$DRRUN -t /bank \
balance "\s*Current balance on Account ([0-9]+):\s*" i \
depositTo "\s*Deposit to Account ([0-9]+)" i \
withdraw "\s*Withdraw from Account ([0-9]+)" i \
transfer "\s*Transfer from Account ([0-9]+) to Account ([0-9]+)" ii \
depositSuccess "^Deposit successful!" $'' \
depositFail "^Deposit amount must be positive!" $'' \
withdrawSuccess "^Withdrawal successful!" $'' \
withdrawFail "^Withdrawal failed!" $'' \
transferSuccess "^Transfer successful!" $'' \
selectedAccount "\s*Selected account: ([0-9]+).*" i \
numOut "^\s*([0-9]+)\s*$" i \
logout "Logged out!" $'' \
-stdin \
numIn "^\s*([0-9]+)\s*$" i \
otherIn ".*[^\s].*" $'' -- $BANK_DIR/bank $@ < /tmp/fifoA  > /tmp/fifoB &
BANK_PID=$!

echo "-- Starting interact --"
cat /tmp/fifoB | $BANK_DIR/interact inputs.last.txt interact.log >/tmp/fifoA &

echo "-- Starting the monitor --"
/usr/bin/time $MONITOR Out:regex:/bank.stdout In:regex:/bank.stdin > mon.stdout 2>mon.stderr &
MON_PID=$!

wait $MON_PID

echo "-- Monitor finished --"

#grep -E 'balancemismatch|balancenegative' mon.stdout
#ERRSNUM=$(grep -E 'balancemismatch|balancenegative' mon.stdout | wc -l)

echo "Errors found: " $(grep -E 'balancemismatch|balancenegative' mon.stdout | wc -l)

rm -f /tmp/fifo{A,B}

cat interact.log
cat mon.stdout
cat mon.stderr
