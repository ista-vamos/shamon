LIBBPF_TOOLS=~/src/bcc/libbpf-tools/

sudo $LIBBPF_TOOLS/readwrite /bank \
balance "\s*Current balance on Account ([0-9]+):\s*" i \
depositTo "\s*Deposit to Account ([0-9]+).*" i \
withdraw "\s*Withdraw from Account ([0-9]+).*" i \
transferTo "\s*Transfer from Account ([0-9]+):.*" i \
transferAmount "\s*Transfer from Account ([0-9]+):.*" i \
depositSuccess "^Deposit successful!.*" $'' \
withdrawSuccess "^Withdrawal successful!.*" $'' \
withdrawFail "^Withdrawal failed!.*" $'' \
transferSuccess "^Transfer successful!.*" $'' \
transferFail "^Transfer failed!.*" $'' \
selectedAccount "\s*Selected account: ([0-9]+).*" i \
invalidAccount "^Invalid account number!.*" $'' \
num "^\s*([0-9]+)\s*$" i \
-stdin \
num "^\s*([0-9]+)\s*$" i -- ./bank.sh $@

#sleep 0.1
#sudo $(dirname $0)/monitor Out:regex:/bank.stdout In:regex:/bank.stdin

#wait
