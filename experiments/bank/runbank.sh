#!/bin/bash
SHAMONBASE="/opt/shamon"
DYNAMORIOBASE="/opt/dynamorio"

set -x
set -e

ORGIRESULTSPATH=$(pwd)
TIME=$(date +%Y%m%d%H%M%S)
RESULTSPATH="bankrun-$TIME"
cd "$SHAMONBASE"
cd experiments
gcc -o3 interact.c -o interact
cd bank
mkdir -p "$RESULTSPATH"
cd "$SHAMONBASE"
for RUNNUM in 1 2 3 4 5 6 7 8 9 10;
do
for shmsize in 1 8 16;
do
  echo "#define SHMBUF_PAGES $shmsize" > ./shmbuf/shmbuf-pagesize.h
  make
	for arbufsize in 16 64 128 512;
	do
	gen/compile.sh mmtest/mmbank.c -DSHMBUF_ARBITER_BUFZIE="$arbufsize"
	mv monitor bankmonitor
	gen/compile.sh mmtest/mmbankempty.c -DSHMBUF_ARBITER_BUFZIE="$arbufsize"
	mv monitor bankemptymonitor
  
    cd "experiments/bank/"

	FIFOA="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-native-A-$RUNNUM"
	FIFOB="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-native-B-$RUNNUM"
	mkfifo "$FIFOA"
	mkfifo "$FIFOB"
	cat "$FIFOB" | ../interact inputs.txt "$RESULTSPATH/bankresult-$arbufsize-$shmsize-native-$RUNNUM.txt" > "$FIFOA" &
	cat "$FIFOA" | ./bank > "$FIFOB" &
	wait
	rm "$FIFOA"
	rm "$FIFOB"

	FIFOA="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-drrio-A-$RUNNUM"
	FIFOB="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-drrio-B-$RUNNUM"
	mkfifo "$FIFOA"
	mkfifo "$FIFOB"
	cat "$FIFOB" | ../interact inputs.txt "$RESULTSPATH/bankresult-$arbufsize-$shmsize-ddrio-$RUNNUM.txt" > "$FIFOA" &
	cat "$FIFOA" | $DYNAMORIOBASE/build/bin64/drrun -root $DYNAMORIOBASE/build/ -- ./bank > "$FIFOB" &
	wait
	rm "$FIFOA"
	rm "$FIFOB"

	FIFOA="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-empty-A-$RUNNUM"
	FIFOB="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-empty-B-$RUNNUM"
	mkfifo "$FIFOA"
	mkfifo "$FIFOB"
	cat "$FIFOB" | ../interact inputs.txt "$RESULTSPATH/bankresult-$arbufsize-$shmsize-empty-$RUNNUM.txt" > "$FIFOA" &
	cat "$FIFOA" | $DYNAMORIOBASE/build/bin64/drrun -root $DYNAMORIOBASE/build/ -c $SHAMONBASE/sources/drsyscalls/libsyscalls.so -- ./bank > "$FIFOB" &
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
	../../bankemptymonitor Out:regexrw:/bank_out In:regexrw:/bank_in | grep -A 5 "DONE!" > "$RESULTSPATH/bankresult-$arbufsize-$shmsize-mon-empty-$RUNNUM.txt" &
	wait
	rm "$FIFOA"
	rm "$FIFOB"

	FIFOA="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-A-$RUNNUM"
	FIFOB="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-B-$RUNNUM"
	mkfifo "$FIFOA"
	mkfifo "$FIFOB"
	cat "$FIFOB" | ../interact inputs.txt "$RESULTSPATH/bankresult-$arbufsize-$shmsize-$RUNNUM.txt" > "$FIFOA" &
	cat "$FIFOA" | $DYNAMORIOBASE/build/bin64/drrun -root $DYNAMORIOBASE/build/ -c $SHAMONBASE/sources/drsyscalls/libsyscalls.so -- ./bank > "$FIFOB" &
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
	../../bankmonitor Out:regexrw:/bank_out In:regexrw:/bank_in | grep -A 5 "DONE!" > "$RESULTSPATH/bankresult-$arbufsize-$shmsize-mon-$RUNNUM.txt" &
	wait
	rm "$FIFOA"
	rm "$FIFOB"
	for errorcount in 5 10 50 100;
	do
		FIFOA="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-$errorcount-native-A-$RUNNUM"
		FIFOB="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-$errorcount-native-B-$RUNNUM"
		mkfifo "$FIFOA"
		mkfifo "$FIFOB"
		cat "$FIFOB" | ../interact inputs.txt "$RESULTSPATH/bankresult-$arbufsize-$shmsize-$errorcount-native-$RUNNUM.txt" > "$FIFOA" &
		cat "$FIFOA" | ./bank errors $errorcount > "$FIFOB" &
	    wait
		rm "$FIFOA"
		rm "$FIFOB"

		FIFOA="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-$errorcount-drrio-A-$RUNNUM"
		FIFOB="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-$errorcount-drrio-B-$RUNNUM"
		mkfifo "$FIFOA"
		mkfifo "$FIFOB"
		cat "$FIFOB" | ../interact inputs.txt "$RESULTSPATH/bankresult-$arbufsize-$shmsize-$errorcount-ddrio-$RUNNUM.txt" > "$FIFOA" &
		cat "$FIFOA" | $DYNAMORIOBASE/build/bin64/drrun -root $DYNAMORIOBASE/build/ -- ./bank errors $errorcount > "$FIFOB" &
	    wait
		rm "$FIFOA"
		rm "$FIFOB"

	    FIFOA="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-$errorcount-empty-A-$RUNNUM"
		FIFOB="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-$errorcount-empty-B-$RUNNUM"
		mkfifo "$FIFOA"
		mkfifo "$FIFOB"
		cat "$FIFOB" | ../interact inputs.txt "$RESULTSPATH/bankresult-$arbufsize-$shmsize-$errorcount-empty-$RUNNUM.txt" > "$FIFOA" &
		cat "$FIFOA" | $DYNAMORIOBASE/build/bin64/drrun -root $DYNAMORIOBASE/build/ -c $SHAMONBASE/sources/drsyscalls/libsyscalls.so -- ./bank errors $errorcount > "$FIFOB" &
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
		../../bankemptymonitor Out:regexrw:/bank_out In:regexrw:/bank_in | grep -A 5 "DONE!" > "$RESULTSPATH/bankresult-$arbufsize-$shmsize-$errorcount-mon-empty-$RUNNUM.txt" &
		wait
		rm "$FIFOA"
		rm "$FIFOB"

		FIFOA="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-$errorcount-A-$RUNNUM"
		FIFOB="$RESULTSPATH/fifoBank-$arbufsize-$shmsize-$errorcount-B-$RUNNUM"
		mkfifo "$FIFOA"
		mkfifo "$FIFOB"
		cat "$FIFOB" | ../interact inputs.txt "$RESULTSPATH/bankresult-$arbufsize-$shmsize-$errorcount-$RUNNUM.txt" > "$FIFOA" &
		cat "$FIFOA" | $DYNAMORIOBASE/build/bin64/drrun -root $DYNAMORIOBASE/build/ -c $SHAMONBASE/sources/drsyscalls/libsyscalls.so -- ./bank errors $errorcount > "$FIFOB" &
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
		../../bankmonitor Out:regexrw:/bank_out In:regexrw:/bank_in | grep -A 5 "DONE!" > "$RESULTSPATH/bankresult-$arbufsize-$shmsize-$errorcount-mon-$RUNNUM.txt" &
		wait
		rm "$FIFOA"
		rm "$FIFOB"
	done
    cd "$SHAMONBASE"
  done
done
done
cd $ORGIRESULTSPATH
