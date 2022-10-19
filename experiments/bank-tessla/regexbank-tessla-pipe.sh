#!/bin/bash

set -e

NUM=$1
shift

DRIOPATH="/opt/vamos/dynamorio/"
DRRUN="$DRIOPATH/build/bin64/drrun\
	-root $DRIOPATH/build/\
	-c ../../sources/drregex/libdrregex-mt.so"

MONITOR=$(dirname $0)/monitor-tessla
if [ $(basename "$0") == "regexbank-dump.sh" ]; then
	MONITOR=$(dirname $0)/../../monitors/monitor-generic
fi

rm -f /tmp/fifo{A,B} /tmp/tessla.in
mkfifo /tmp/fifo{A,B} /tmp/tessla.in

python3 inputs.py $NUM > inputs.last.txt

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
otherIn ".*[^\s].*" $'' -- $(dirname $0)/bank $@ < /tmp/fifoA  > /tmp/fifoB 2>source-log.txt &

echo "-- Starting the monitor --"
$MONITOR Out:regex:/bank.stdout In:regex:/bank.stdin&
MON_PID=$!

# wait for the source and monitor to be ready
tail -n 1000 -f source-log.txt | while read line; do
    if echo $line | grep -L "Continue program"; then
            echo "All ready!"
            break
    fi
done

echo "-- Starting interact --"
cat /tmp/fifoB | ./interact inputs.last.txt interact.log >/tmp/fifoA &

echo "-- Starting TeSSLa -- "
java -jar /opt/vamos/tessla/target/scala-2.13/tessla-assembly-1.2.4.jar interpreter --base-time 1ns spec.tessla /tmp/tessla.in
TESSLA_PID=$!

wait $TESSLA_PID

rm -f /tmp/fifo{A,B} /tmp/tessla.in
pkill -9 monitor-tessla
pkill -9 cat
pkill -9 bank

cat interact.log
