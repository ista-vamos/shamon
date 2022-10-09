LIBBPF_TOOLS=$HOME/src/bcc/libbpf-tools/
MONITOR=$(dirname $0)/monitor
BANK=./bank.sh

#MONITOR=$(dirname $0)/../../monitors/monitor-generic
#BANK=-p

sudo $LIBBPF_TOOLS/readwrite /bank \
balance "\s*Current balance on Account ([0-9]+):\s*" i \
depositTo "\s*Deposit to Account ([0-9]+).*" i \
withdraw "\s*Withdraw from Account ([0-9]+).*" i \
transfer "\s*Transfer from Account ([0-9]+) to Account ([0-9]+).*" ii \
depositSuccess "^Deposit successful!.*" $'' \
depositFail "^Deposit amount must be positive!.*" $'' \
withdrawSuccess "^Withdrawal successful!.*" $'' \
withdrawFail "^Withdrawal failed!.*" $'' \
transferSuccess "^Transfer successful!.*" $'' \
selectedAccount "\s*Selected account: ([0-9]+).*" i \
readInput "\s*Select account no.*|Select Action:.*|Press ENTER.*|Transfer from Account [0-9]+:.*|Receiving Account:.*|Changing account..*|.*Logged out!$" $'' \
numOut "^\s*([0-9]+)\s*$" i \
login ".*Login:.*Password:.*" $'' \
-stdin \
numIn "^\s*([0-9]+)\s*$" i \
otherIn ".*" $'' -- $BANK $@ 1>stdout.log &

sudo $MONITOR Out:regex:/bank.stdout In:regex:/bank.stdin
wait
