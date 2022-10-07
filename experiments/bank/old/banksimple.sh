mkfifo /tmp/fifo{A,B}
cat /tmp/fifoB | ../interact inputs.txt > /tmp/fifoA &
cat /tmp/fifoA | ./bank > /tmp/fifoB &
wait
rm /tmp/fifoA
rm /tmp/fifoB
