LIBBPF_TOOLS=$HOME/src/bcc/libbpf-tools/
MONITOR=$(dirname $0)/monitor
BANK=./bank.sh

MONITOR=$(dirname $0)/../../monitors/monitor-generic

sudo $LIBBPF_TOOLS/readwrite /bank \
balance "\s*Current balance on Account ([0-9]+):\s*" i \
depositTo "\s*Deposit to Account ([0-9]+).*" i \
withdraw "\s*Withdraw from Account ([0-9]+).*" i \
transferFrom "\s*Transfer from Account ([0-9]+):.*" i \
depositSuccess "^Deposit successful!.*" $'' \
depositFail "^Deposit amount must be positive!.*" $'' \
withdrawSuccess "^Withdrawal successful!.*" $'' \
withdrawFail "^Withdrawal failed!.*" $'' \
transferSuccess "^Transfer successful!.*" $'' \
transferFail "^Transfer failed!.*" $'' \
selectedAccount "\s*Selected account: ([0-9]+).*" i \
invalidAccount "^Invalid account number!.*" $'' \
loggedOut "^Logged out!.*" $'' \
numOut "^\s*([0-9]+)\s*$" i \
-stdin \
numIn "^\s*([0-9]+)\s*$" i -- $BANK $@ 1>stdout.log &

sudo $MONITOR Out:regex:/bank.stdout In:regex:/bank.stdin

wait
