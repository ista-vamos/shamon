mkfifo /tmp/fifo{A,B}
DRIOPATH=~/dynamorio
cat /tmp/fifoB | ../interact inputs.txt > /tmp/fifoA &
cat /tmp/fifoA | $DRIOPATH/build/bin64/drrun -root $DRIOPATH/build/ -c ../../drsyscalls/libsyscalls.so -- ./bank errors 10 > /tmp/fifoB &
BANK_PID=$!
sleep 0.1
../../sources/regexdrw $BANK_PID /bank $'>' \
balance "\s*Current balance on Account ([0-9]+):\s*([0-9]+).*" ii \
deposit "\s*Deposit to Account ([0-9]+).*" i \
withdraw "\s*Withdraw from Account ([0-9]+).*" i \
transferTo "\s*Transfer from Account ([0-9]+):.*" i \
transferAmount "\s*Transfer from Account ([0-9]+)\s*to Account ([0-9]+).*" ii \
depositSuccess "\s*Welcome, [a-zA-Z0-9]+!\s*Deposit successful!.*" $'' \
withdrawSuccess "\s*Welcome, [a-zA-Z0-9]+!\s*Withdrawal successful!.*" $'' \
withdrawFail "\s*Welcome, [a-zA-Z0-9]+!\s*Withdrawal failed!.*" $'' \
transferSuccess "\s*Welcome, [a-zA-Z0-9]+!\s*Transfer successful!.*" $'' \
transferFail "\s*Welcome, [a-zA-Z0-9]+!\s*Transfer failed!.*" $'' \
invalidAccount "\s*Welcome, [a-zA-Z0-9]+!\s*Unknown account!.*" $'' \
otherOut ".*" $'' \
-- $'\n' \
num "^\s*([0-9]+)\s*" i \
otherIn ".*" $'' &
sleep 0.1
#../../monitor Out:regexrw:/bank_out In:regexrw:/bank_in &
wait
rm /tmp/fifoA
rm /tmp/fifoB
