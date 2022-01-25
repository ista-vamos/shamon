mkfifo /tmp/fifo{A,B}
DRIOPATH=~/dynamorio
cat /tmp/fifoB | ../interact inputs.txt > /tmp/fifoA &
cat /tmp/fifoA | $DRIOPATH/build/bin64/drrun -root $DRIOPATH/build/ -c ../../drsyscalls/libsyscalls.so -- ./bank > /tmp/fifoB &
BANK_PID=$!
./monitor $BANK_PID &
wait
rm /tmp/fifoA
rm /tmp/fifoB
