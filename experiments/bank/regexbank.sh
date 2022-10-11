set -e

NUM=$1
shift

LIBBPF_TOOLS=$HOME/src/bcc/libbpf-tools/
MONITOR=$(dirname $0)/monitor
#MONITOR=$(dirname $0)/../../monitors/monitor-generic

rm -f /tmp/fifo{A,B}
mkfifo /tmp/fifo{A,B}

python inputs.py $NUM > inputs.last.txt

$(dirname $0)/bank $@ < /tmp/fifoA  > /tmp/fifoB &
BANK_PID=$!

echo "Bank PID: $BANK_PID"

sudo $LIBBPF_TOOLS/readwrite -t /bank \
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
otherIn ".*[^\s].*" $'' -- -p $BANK_PID 2> source-log.txt &
SRC_PID=$!

echo "-- Starting the monitor --"
sudo $MONITOR Out:regex:/bank.stdout In:regex:/bank.stdin&
MON_PID=$!

# wait for the source and monitor to be ready
tail -n 1000 -f source-log.txt | while read line; do
    if echo $line | grep -q "Entering the main loop"; then
	    echo "All ready!"
	    break
    fi
done

echo "-- Starting interact --"
cat /tmp/fifoB | ./interact inputs.last.txt interact.log >/tmp/fifoA &

wait $MON_PID

rm -f /tmp/fifo{A,B}

if grep -q "DROPPED" source-log.txt; then
	echo -e "\033[31mWARNING: Source dropped some events!\033[0m"
fi

grep 'Errors generated' interact.log
