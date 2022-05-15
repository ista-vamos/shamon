DRIOPATH=~/dynamorio
SHAMONPATH=~/shamon
PRIMESPATH=~/experiments/primes
$DRIOPATH/build/bin64/drrun -root $DRIOPATH/build/ -c $SHAMONPATH/drsyscalls/libsyscalls.so -- $PRIMESPATH/primes 100000 >primes1out.txt &
PRIMES1_PID=$!
$DRIOPATH/build/bin64/drrun -root $DRIOPATH/build/ -c $SHAMONPATH/drsyscalls/libsyscalls.so -- python3 $PRIMESPATH/primes.py 100000 >primes2out.txt &
PRIMES2_PID=$!
$SHAMONPATH/sources/regexd $PRIMES1_PID /primes1 prime "\s*\#([0-9]+):\s+([0-9]+)\s*" ii >primes1regout.txt &
$SHAMONPATH/sources/regexd $PRIMES2_PID /primes2 prime "\s*\#([0-9]+):\s+([0-9]+)\s*" ii >primes2regout.txt &
$SHAMONPATH/monitor Left:regex:/primes1 Right:regex:/primes2 &
wait
