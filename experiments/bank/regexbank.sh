#!/bin/bash

set -e

NUM=$1
shift

DRIOPATH="/opt/vamos/dynamorio/"
DRRUN="$DRIOPATH/build/bin64/drrun\
	-root $DRIOPATH/build/\
	-c ../../sources/drregex/libdrregex-mt.so"

MONITOR=$(dirname $0)/monitor-vamos
if [ $(basename "$0") == "regexbank-dump.sh" ]; then
	MONITOR=$(dirname $0)/../../monitors/monitor-generic
fi

rm -f /tmp/fifo{A,B}
mkfifo /tmp/fifo{A,B}

python inputs.py $NUM > inputs.last.txt

# we can start it a bit before as it is waiting for the connection
echo "-- Starting the monitor --"
$MONITOR Out:regex:/bank.stdout In:regex:/bank.stdin&
MON_PID=$!

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
otherIn ".*[^\s].*" $'' -- $(dirname $0)/bank $@ < /tmp/fifoA  > /tmp/fifoB &
BANK_PID=$!

echo "-- Starting interact --"
cat /tmp/fifoB | ./interact inputs.last.txt interact.log >/tmp/fifoA &

wait $MON_PID

rm -f /tmp/fifo{A,B}

cat interact.log
